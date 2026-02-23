#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>
#include <set>
#include <string>

#include "include/search.h"

using namespace std;
using namespace testing;

namespace {

TEST(BuildIndex, TinierTxt) {
  string filename = "data/tinier.txt";
  map<string, set<string>> expectedIndex = {
      {"to", {"www.example.com", "www.otherexample.com"}},
      {"be", {"www.example.com", "www.otherexample.com"}},
      {"ora", {"www.example.com"}},
      {"not", {"www.example.com"}},
      {"want", {"www.otherexample.com"}},
      {"free", {"www.otherexample.com"}},
  };
  map<string, set<string>> studentIndex;
  int studentNumProcesed = buildIndex(filename, studentIndex);

  string indexTestFeedback =
      "buildIndex(\"" + filename + "\", ...) index incorrect\n";
  EXPECT_THAT(studentIndex, ContainerEq(expectedIndex)) << indexTestFeedback;

  string retTestFeedback =
      "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
  EXPECT_THAT(studentNumProcesed, Eq(2)) << retTestFeedback;
}

}  // namespace
