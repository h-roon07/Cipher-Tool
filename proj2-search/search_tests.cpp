#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/search.h"

using namespace std;
using namespace testing;

/*
TEST(FakeTest, PleaseDeleteOnceYouWriteSome) {
  // If no tests exist, GoogleTest fails to compile with a fairly confusing
  // message. We have this empty test that does nothing to allow an empty
  // autograder submission to compile and regularly fail. Once you write your
  // own tests for the project, you can delete this one.

  
  EXPECT_THAT(1, Eq(1));
}
*/

// Milestone 1
// CleanToken
TEST(CleanToken, LowercaseConversion) {
  EXPECT_THAT(cleanToken("Compsci"), StrEq("compsci"));
  EXPECT_THAT(cleanToken("UIC"), StrEq("uic"));
  EXPECT_THAT(cleanToken("Haroon"), StrEq("haroon"));
}

TEST(CleanToken, PunctuationSides) {
  EXPECT_THAT(cleanToken("...Chicago..."), StrEq("chicago"));
  EXPECT_THAT(cleanToken("???Illinois???"), StrEq("illinois"));
  EXPECT_THAT(cleanToken("///States///"), StrEq("states"));
}

TEST(CleanToken, PunctuationMiddle) {
  EXPECT_THAT(cleanToken("one:one"), StrEq("one:one"));
  EXPECT_THAT(cleanToken("cs-251"), StrEq("cs-251"));
  EXPECT_THAT(cleanToken("chocolate>vanilla"), StrEq("chocolate>vanilla"));
}

TEST(CleanToken, PunctuationMiddleAndStartNotEnd) {
  EXPECT_THAT(cleanToken("...cs-251"), StrEq("cs-251"));
  EXPECT_THAT(cleanToken("///one:one"), StrEq("one:one"));
}

TEST(CleanToken, PunctuationMiddleAndEndNotStart) {
  EXPECT_THAT(cleanToken("cs-251..."), StrEq("cs-251"));
  EXPECT_THAT(cleanToken("one:one///"), StrEq("one:one"));
}


// GatherTokens
TEST(GatherTokens, LeadingSpaces) {
  string text = "   UIC CS";
  set<string> expected = {"uic", "cs"};
  EXPECT_THAT(gatherTokens(text), ContainerEq(expected));
}

TEST(GatherTokens, TrailingSpaces) {
  string text = "haroon azhar   ";
  set<string> expected = {"haroon", "azhar"};
  EXPECT_THAT(gatherTokens(text), ContainerEq(expected));
}

TEST(GatherTokens, SpacesBetweenWords) {
  string text = "CS251   Project   Test";
  set<string> expected = {"cs251", "project", "test"};
  EXPECT_THAT(gatherTokens(text), ContainerEq(expected));
}


// Buildindex
TEST(BuildIndex, TinytxtCheck) {
  map <string, set<string>> index;
  int pages = buildIndex("data/tiny.txt", index);
  EXPECT_THAT(pages, Eq(4));
  EXPECT_THAT(index["eggs"], ContainerEq(set<string>{"www.shoppinglist.com"}));
  EXPECT_THAT(index["fish"], ContainerEq(set<string>{"www.shoppinglist.com", "www.dr.seuss.net"}));
  EXPECT_THAT(index["gre-en"], ContainerEq(set<string>{"www.rainbow.org"}));
  EXPECT_THAT(index["to"], ContainerEq(set<string>{"www.bigbadwolf.com"}));
}

TEST(BuildIndex, TinytxtReturnindex) {
  map <string, set<string>> index;
  int pages = buildIndex("data/tiny.txt", index);
  EXPECT_THAT(pages, Eq(4));
}

TEST(BuildIndex, FileNotFound) {
  map <string, set<string>> index;
  int pages = buildIndex("test_dummy.txt", index);
  EXPECT_THAT(pages, Eq(0));
}



// Milestone 2
// FindQueryMatches
TEST(FindQueryMatches, FirstTermNotInindex) {
  map<string, set<string>> index = {{"uic", {"a.com", "b.com"}}, {"cs", {"b.com"}}}; 
  EXPECT_THAT(findQueryMatches(index, "missing"), ContainerEq(set<string> {}));
}

TEST(FindQueryMatches, LaterTermNotInindex) {
  map<string, set<string>> index = {{"uic", {"a.com", "b.com"}}, {"cs", {"b.com"}}}; 
  EXPECT_THAT(findQueryMatches(index, "uic missing"), ContainerEq(set<string> {"a.com", "b.com"}));
  EXPECT_THAT(findQueryMatches(index, "uic +missing"), ContainerEq(set<string> {}));
  EXPECT_THAT(findQueryMatches(index, "uic -missing"), ContainerEq(set<string> {"a.com", "b.com"}));
} 
