#include "include/search.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string cleanToken(const string& token) {
  string s = token;
  for (int i=0; i < s.size(); i++) {
    s[i] = tolower(s[i]);
  }

  bool hasLetter = false;
  for (int i=0; i < s.size(); i++) {
    if (isalpha(s[i])) {
      hasLetter = true;
      break;
    }
  }
  if (!hasLetter) {
    return "";
 }
  
  int left = 0;
  while (left < s.size() && ispunct(s[left])) {
    left++;
  }

  int right = s.size()-1;
  while (right > left && ispunct(s[right])) {
    right--;
  }

  return s.substr(left, right - left + 1);
}



set<string> gatherTokens(const string& text) {
  set<string> tokens;
  stringstream s(text);
  string word;
  while (s >> word) {
    string fixed = cleanToken(word);
    if (fixed != "") {
      tokens.insert(fixed);
    }
  }
  return tokens;
}

int buildIndex(const string& filename, map<string, set<string>>& index) {
  ifstream fin(filename);
  int count = 0;
  string link;
  string text;

  while (getline(fin, link)) {
    if (!getline(fin, text)) {
      break;
    }
    count++;
    set<string> words = gatherTokens(text);
    for (string word : words) {
      index[word].insert(link);
    }
  }
  return count;
}

set<string> findQueryMatches(const map<string, set<string>>& index, const string& sentence) {
  stringstream queryStream(sentence);
  string rawQuery;
  set<string> result;
  bool first = true;

  while (queryStream >> rawQuery) {
    char mod = 0;
    if (rawQuery[0] == '+' || rawQuery[0] == '-') {
      mod = rawQuery[0];
      rawQuery = rawQuery.substr(1);
    }

    string cleaned = cleanToken(rawQuery);
    set<string> matchingPages;
    auto found = index.find(cleaned);
    if (found != index.end()) {
      matchingPages = found->second;
    }

    if (first) {
      result = matchingPages;
      first = false;
      continue;
    }

    set<string> newResult;
    if (mod == '+') {
      for (string link : result) {
        if (matchingPages.find(link) != matchingPages.end()) {
          newResult.insert(link);
        }
      }
    }

    else if (mod == '-') {
      for (string link : result) {
        if (matchingPages.find(link) == matchingPages.end()) {
          newResult.insert(link);
        }
      }
    } else {
      newResult = result;
      for (string link : matchingPages) {
        newResult.insert(link);
      }
    }
    result = newResult;
  }

  return result;
}

void searchEngine(const string& filename) {
  ifstream fin(filename);
   if (!fin) {
     cout << "Invalid filename." << endl;
   }
  cout << "Stand by while building index..." << endl;
  map<string, set<string>> index;
  int pages = buildIndex(filename, index);
  cout << "Indexed " << pages << " pages containing " << index.size() << " unique terms" << endl;

  while(true) {
    string query;
    cout << "Enter query sentence (press enter to quit): ";
    getline(cin, query);
    if (query == "") {
      cout << "Thank you for searching!" << endl;
      break;
    }
    set<string> matches = findQueryMatches(index, query);
    cout << "Found " << matches.size() << " matching pages" << endl;
    for (string link : matches) {
      cout << link << endl;
    }
  }
}
