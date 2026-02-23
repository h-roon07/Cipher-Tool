#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function declarations go at the top of the file so we can call them
// anywhere in our program, such as in main or in other functions.
// Most other function declarations are in the included header
// files.

// When you add a new helper function, make sure to declare it up here!

/**
 * Print instructions for using the program.
 */
void printMenu();
double scoreString(const QuadgramScorer& scorer, const string& s);
vector<char> hillClimb(const QuadgramScorer& scorer, const string& ciphertext);
void computeEnglishnessCommand(const QuadgramScorer& scorer);
vector<char> decryptSubstCipher(const QuadgramScorer& scorer, const string& ciphertext);
void decryptSubstCipherCommand(const QuadgramScorer& scorer);

int main() {
  Random::seed(time(NULL));
  string command;

  vector<string> quadgrams;
  vector<int> counts;
  string line;
  ifstream quadgramFile("english_quadgrams.txt");

  while (getline(quadgramFile, line)) {
    if (line.empty()) {
      continue;
    }

    int commaPos = line.find(',');
    if (commaPos == string::npos) {
      continue;
    }

    string quadgram = line.substr(0, commaPos);
    string count = line.substr(commaPos+1);
    quadgrams.push_back(quadgram);
    counts.push_back(stoi(count));

  }

  QuadgramScorer scorer(quadgrams, counts);

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }
    
    if (command == "C" || command == "c") {
      caesarEncryptCommand();
    }

    if (command == "D" || command == "d") {
      vector<string> dict;
      ifstream fin("dictionary.txt");
      string word;
      while (getline(fin, word)) {
        if (!word.empty()) {
          dict.push_back(word);
        }
      }

      caesarDecryptCommand(dict);
    }

    if (command == "E" || command == "e") {
      computeEnglishnessCommand(scorer);
    }

    if (command == "F" || command == "f") {
      string ciphertext;
      string inputf;
      string outputf;
      getline(cin, inputf);
      getline(cin, outputf);

      ifstream fin(inputf);
      string line;
      while (getline(fin, line)) {
        ciphertext += line;
        ciphertext += '\n';
      }

      vector<char> key = decryptSubstCipher(scorer, ciphertext);
      string text = applySubstCipher(key, ciphertext);
      ofstream fout(outputf);
      fout << text;
    }

    if (command == "S" || command == "s") {
      decryptSubstCipherCommand(scorer);
    }

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  int index = ALPHABET.find(c);
  int newIndex = (index + amount) % 26;
  return ALPHABET[newIndex];
}

string rot(const string& line, int amount) {
  string result;
  for (char c : line) {
    if (isalpha(c)) {
      char uppercase = toupper(c);
      result += rot(uppercase, amount);
    } else if (isspace(c)) {
      result += c;
    }
  }
  return result;
}

void caesarEncryptCommand() {
  string line;
  string inputAmount;

  cout << "Enter the text to encrypt: ";
  getline(cin, line);

  cout << "Enter the number of characters to rotate by: ";
  getline(cin, inputAmount);

  int amountRot = stoi(inputAmount) % 26;
  string encrypted = rot(line, amountRot);
  cout << encrypted << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  for (int i = 0; i < strings.size(); i++) {
    strings[i] = rot(strings[i], amount);
  }
}

string clean(const string& s) {
  string output;
  for (char c : s) {
    if (isalpha(c)) {
      output += (char)toupper(c);
    }
  }
  return output;
}

vector<string> splitBySpaces(const string& s) {
  vector<string> words;
  int start = 0;
  int sLength = s.length();

  while (start < sLength) {
    while (start < sLength && s[start] == ' ') {
      start++;
    }

    if (start >= sLength) {
      break;
    }

    int end = start;
    while (end < sLength && s[end] != ' ') {
      end++;
    }

    words.push_back(s.substr(start, end-start));
    start = end + 1;
  }

  return words;
}

string joinWithSpaces(const vector<string>& words) {
  if (words.empty()) {
    return "";
  }

  string output = words[0];
  for (int i = 1; i < words.size(); i++) {
    output += " ";
    output += words[i];
  }
  return output;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int count = 0;
  for (int i = 0; i < words.size(); i++) {
    for (int j = 0; j < dict.size(); j++) {
      if (words[i] == dict[j]) {
        count++;
        break;
      }
    }
  }
  return count;
}

void caesarDecryptCommand(const vector<string>& dict) {
  cout << "Enter the text to Caesar decrypt: ";
  string encryptedText;
  getline(cin, encryptedText);
  vector<string> cleanWords;
  vector<string> splitWords = splitBySpaces(encryptedText);

  for (int i = 0; i < splitWords.size(); i++) {
    cleanWords.push_back(clean(splitWords[i]));
  }

  bool foundDecryption = false;
  for (int i = 0; i < 26; i++) {
    vector<string> decryptedWords = cleanWords;
    rot(decryptedWords, i);

    int validWordCount = numWordsIn(decryptedWords, dict);
    int wordCount = decryptedWords.size();
    if (validWordCount > (wordCount/2)) {
      cout << joinWithSpaces(decryptedWords) << endl;
      foundDecryption = true;
    }
  }

  if (!foundDecryption) {
    cout << "No good decryptions found" << endl;
  }


}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string output;

  for (char c : s) {
    if (isalpha(c)) {
      char uppercase = (char)toupper(c);
      int index = ALPHABET.find(uppercase);
      output += cipher.at(index);
    } else {
      output += c;
    }
  }

  return output;
}

void applyRandSubstCipherCommand() {
  string text;
  getline(cin, text);
  vector<char> randomCipher = genRandomSubstCipher();
  cout << applySubstCipher(randomCipher, text);
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  double totalScore = 0.0;
  for (int i = 0; i <= s.length()-4; i++) {
    string quadgram = s.substr(i, 4);
    totalScore += scorer.getScore(quadgram);
  }

  return totalScore; 
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  cout << "Enter a string for englishness scoring:";
  string input;
  getline(cin, input);
  double score = 0.0;
  string cleaned = clean(input);
  
  if (cleaned.length() >= 4) {
    score = scoreString(scorer, cleaned);
  }
  cout << score << endl;
}

vector<char> hillClimb(const QuadgramScorer& scorer, const string& ciphertext) {
  vector<char> key = genRandomSubstCipher();
  vector<pair<int, int>> swapPairs;

  for (int i = 0; i < 26; i++) {
    for (int j = i + 1; j < 26; j++) {
      swapPairs.push_back({i, j});
    }
  }

  string decrypted = applySubstCipher(key, ciphertext);
  string cleaned = clean(decrypted);
  double currentScore = 0.0;
  if (cleaned.length() >= 4) {
    currentScore = scoreString(scorer, cleaned);
  }

  bool improved = true;
  while (improved) {
    improved = false;
    for (int i = 0; i < swapPairs.size(); i++) {
      int a = swapPairs[i].first;
      int b = swapPairs[i].second;
      swap(key[a], key[b]);
      
      string attemptDecrypted = applySubstCipher(key, ciphertext);
      string attemptCleaned = clean(attemptDecrypted);
      double newScore = 0.0;

      if (attemptCleaned.length() >= 4) {
        newScore = scoreString(scorer, attemptCleaned);
      }

      if (newScore > currentScore) {
        currentScore = newScore;
        improved = true;
        break;
      } 
      swap(key[a], key[b]);
    }
  }

  return key;
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer, const string& ciphertext) {
  vector<char> bestKey;
  double bestScore = 0.0;

  for (int i = 0; i < 25; i++) {
    vector<char> key = hillClimb(scorer, ciphertext);
    string decrypted = applySubstCipher(key, ciphertext);
    string cleaned = clean(decrypted);
    double score = 0.0;

    if (cleaned.length() >= 4) {
      score = scoreString(scorer, cleaned);
    }

    if (i == 0 || score > bestScore) {
      bestScore = score;
      bestKey = key;
    }
  }

  return bestKey;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  string input;
  getline(cin, input);
  vector<char> key = decryptSubstCipher(scorer, input);
  string output = applySubstCipher(key, input);
  cout << output << endl;
}

#pragma endregion SubstDec
