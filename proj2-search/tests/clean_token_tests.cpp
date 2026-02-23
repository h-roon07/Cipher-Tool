#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/search.h"

using namespace std;
// GoogleTest's functions are in the "testing" namespace.
using namespace testing;

namespace {

// This could be better written using one large parameterized test. However,
// this file is being used as an example for to read and mimic, so we limit
// the "special" / "advanced" tools we use.

// Tokens without any punctuation or special characters
TEST(CleanToken, NoCleaning) {
  ASSERT_THAT(cleanToken("same"), StrEq("same"));
  ASSERT_THAT(cleanToken("wander"), StrEq("wander"));
  ASSERT_THAT(cleanToken("l33tcode"), StrEq("l33tcode"));
}

// Tokens with punctuation at the beginning
TEST(CleanToken, PrefixCleaning) {
  ASSERT_THAT(cleanToken(".hello"), StrEq("hello"));
  ASSERT_THAT(cleanToken("...hello"), StrEq("hello"));
  ASSERT_THAT(cleanToken(".\"!?hello"), StrEq("hello"));
  ASSERT_THAT(cleanToken(";timesheet"), StrEq("timesheet"));
  ASSERT_THAT(cleanToken(";.!timesheet"), StrEq("timesheet"));
  ASSERT_THAT(cleanToken(".,.!?timesheet"), StrEq("timesheet"));
}

// Tokens with punctuation at the end
TEST(CleanToken, SuffixCleaning) {
  ASSERT_THAT(cleanToken("hello."), StrEq("hello"));
  ASSERT_THAT(cleanToken("hello..."), StrEq("hello"));
  ASSERT_THAT(cleanToken("hello.\"!?"), StrEq("hello"));

  ASSERT_THAT(cleanToken("timesheet;"), StrEq("timesheet"));
  ASSERT_THAT(cleanToken("timesheet;.!"), StrEq("timesheet"));
  ASSERT_THAT(cleanToken("timesheet.,.!?"), StrEq("timesheet"));
}

// Tokens without any letters
TEST(CleanToken, ToEmpty) {
  ASSERT_THAT(cleanToken("23432423"), StrEq(""));
  ASSERT_THAT(cleanToken("2343098765432345678998765434567892423"), StrEq(""));
  ASSERT_THAT(cleanToken("....$$$$......"), StrEq(""));
  ASSERT_THAT(cleanToken("....2312^#@@@...."), StrEq(""));
  ASSERT_THAT(cleanToken(""), StrEq(""));
}

// Tokens with uppercase letters and punctuation
TEST(CleanToken, Uppercase) {
  ASSERT_THAT(cleanToken("HELLO."), StrEq("hello"));
  ASSERT_THAT(cleanToken("heLlo..."), StrEq("hello"));
  ASSERT_THAT(cleanToken("hellO.\"!?"), StrEq("hello"));
  ASSERT_THAT(cleanToken(".HELLO"), StrEq("hello"));
  ASSERT_THAT(cleanToken("...Hello"), StrEq("hello"));
  ASSERT_THAT(cleanToken(".\"!?heLLo"), StrEq("hello"));
}

}  // namespace
