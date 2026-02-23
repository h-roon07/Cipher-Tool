#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>
#include <set>
#include <string>

#include "include/search.h"

using namespace std;
using namespace testing;

namespace {

map<string, set<string>> INDEX = {
    {"hello", {"example.com", "uic.edu"}},
    {"there", {"example.com"}},
    {"according", {"uic.edu"}},
    {"to", {"uic.edu"}},
    {"all", {"example.com", "uic.edu", "random.org"}},
    {"known", {"uic.edu"}},
    {"laws", {"random.org"}},
    {"of", {"random.org"}},
    {"aviation", {"random.org"}},
    {"a", {"uic.edu", "random.org"}},
};

TEST(FindQueryMatches, SingleTerm) {
  set<string> expected;

  expected = {"example.com"};
  EXPECT_THAT(findQueryMatches(INDEX, "there"), ContainerEq(expected));

  expected = {"example.com", "uic.edu"};
  EXPECT_THAT(findQueryMatches(INDEX, "hello"), ContainerEq(expected));
  EXPECT_THAT(findQueryMatches(INDEX, "Hello!"), ContainerEq(expected));
}

TEST(FindQueryMatches, Union) {
  set<string> expected;

  expected = {"uic.edu"};
  EXPECT_THAT(findQueryMatches(INDEX, "known to"), ContainerEq(expected));

  expected = {"example.com", "uic.edu", "random.org"};
  EXPECT_THAT(findQueryMatches(INDEX, "hello laws aviation"),
              ContainerEq(expected));
  EXPECT_THAT(findQueryMatches(INDEX, "Hello, Laws, AVIATION!"),
              ContainerEq(expected));
}

TEST(FindQueryMatches, Intersection) {
  set<string> expected;

  expected = {"example.com"};
  EXPECT_THAT(findQueryMatches(INDEX, "all +there"), ContainerEq(expected));

  expected = {"example.com", "uic.edu"};
  EXPECT_THAT(findQueryMatches(INDEX, "hello +all"), ContainerEq(expected));

  expected = {"uic.edu"};
  EXPECT_THAT(findQueryMatches(INDEX, "hello +all +to"), ContainerEq(expected));

  expected = {};
  EXPECT_THAT(findQueryMatches(INDEX, "hello +aviation"),
              ContainerEq(expected));
}

TEST(FindQueryMatches, Difference) {
  set<string> expected;

  expected = {"example.com", "uic.edu"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -laws"), ContainerEq(expected));

  expected = {"random.org"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -hello"), ContainerEq(expected));

  expected = {"example.com"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -of -a"), ContainerEq(expected));

  expected = {};
  EXPECT_THAT(findQueryMatches(INDEX, "known -to"), ContainerEq(expected));

  expected = {};
  EXPECT_THAT(findQueryMatches(INDEX, "to -all -a"), ContainerEq(expected));
}

TEST(FindQueryMatches, LongCombinedQueries) {
  set<string> expected;

  expected = {"example.com"};
  EXPECT_THAT(findQueryMatches(INDEX, "there laws aviation -to +hello"),
              ContainerEq(expected));

  expected = {};
  EXPECT_THAT(findQueryMatches(INDEX, "all -all +all"), ContainerEq(expected));

  expected = {"example.com", "uic.edu", "random.org"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -all +all all"),
              ContainerEq(expected));
}

}  // namespace
