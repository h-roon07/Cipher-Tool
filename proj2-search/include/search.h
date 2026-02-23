#pragma once

#include <map>
#include <set>
#include <string>

using namespace std;

/**
 * Cleans the provided `token` by returning a new string with all leading and
 * trailing punctuation removed. If the `token` contains no letters, returns
 * the empty string to indicate that it should be discarded.
 *
 * Assumes that `token` contains no spaces.
 */
string cleanToken(const string& token);

/**
 * Splits the input text into words ("tokens"), cleans each token, and returns
 * the set of all tokens in the text.
 */
set<string> gatherTokens(const string& text);

/**
 * Builds an index from the contents of the given file. The lines in the file
 * come in pairs: a URL, and its corresponding text. For each text, the function
 * extracts and cleans all tokens, then associates each token with the URL in
 * the provided index.
 *
 * `index` is an inverted index, mapping tokens to the URLs they appear for.
 */
int buildIndex(const string& filename, map<string, set<string>>& index);

/**
 * Finds matching URLs for a given query sentence by looking up each token in
 * the provided index. The sentence is parsed from left to right, and each token
 * lookup is combined using either set union (no modifier), set intersection
 * (+), or set difference (-).
 *
 * Assumes the sentence is "well-formed" with at least one search term.
 */
set<string> findQueryMatches(const map<string, set<string>>& index,
                             const string& sentence);

/**
 * Given a filename, builds an index from the file and repeatedly prompts the
 * user for search queries. For each query, outputs the number of matching pages
 * and the URLs found. Exits when the user presses enter without entering a
 * query.
 */
void searchEngine(const string& filename);
