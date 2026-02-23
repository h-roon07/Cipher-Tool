#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <set>
#include <string>

#include "include/search.h"

using namespace std;
using namespace testing;

namespace {

TEST(GatherTokens, BasicTest) {
  string text = "to be or not to be";
  set<string> expected = {"to", "be", "or", "not"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(GatherTokens, Nom) {
  string text = "I'm not trying to eat you!";
  set<string> expected = {"i'm", "not", "trying", "to", "eat", "you"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(GatherTokens, Colors) {
  string text = "red ~gre-en~ orange, yellow + blue (indigo) &violet";
  set<string> expected = {"red",    "gre-en", "orange", "blue",
                          "indigo", "violet", "yellow"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

}  // namespace
