#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <regex>
#include <string>
#include <vector>

#include "include/search.h"

using namespace std;
using namespace testing;

namespace {

// GoogleTest uses printf, which doesn't rely on cout -- so capturing
// works fine!
class CaptureCinCout : public testing::Test {
 protected:
  std::stringstream input;
  std::stringstream output;

  std::streambuf* saved_cin{};
  std::streambuf* saved_cout{};

  CaptureCinCout() {
    saved_cin = std::cin.rdbuf();
    saved_cout = std::cout.rdbuf();
    std::cin.rdbuf(input.rdbuf());
    std::cout.rdbuf(output.rdbuf());
  }

  ~CaptureCinCout() {
    std::cin.rdbuf(saved_cin);
    std::cout.rdbuf(saved_cout);
    saved_cin = nullptr;
    saved_cout = nullptr;
  }

  // Debugging tool to exfiltrate despite redirection
  // Could also use printf
  void write_to_cout(std::string s) {
    saved_cout->sputn(s.c_str(), s.length());
  }
};

// This should be a matcher, but I couldn't figure it out
bool strWhitespaceEq(string actual, string expected) {
  // Account for ending newline
  actual.push_back('\n');
  expected.push_back('\n');
  string expected_ = regex_replace(expected + "\n", regex("\\s+"), "_");
  string actual_ = regex_replace(actual + "\n", regex("\\s+"), "_");
  return actual_ == expected_;
}

class SearchEngine : public CaptureCinCout {};

TEST_F(SearchEngine, MissingFile) {
  input << "query" << endl;
  input << endl;
  input << endl;
  searchEngine("missingfile.txt");

  vector<string> expectedLines = {
      // Response to missing file
      "Invalid filename.",
      // Info output
      "Stand by while building index...",
      "Indexed 0 pages containing 0 unique terms",
      // Prompt, response to "query"
      "Enter query sentence (press enter to quit): Found 0 matching pages",
      // Prompt, exit message for ""
      "Enter query sentence (press enter to quit): Thank you for searching!",
  };
  ostringstream oss;
  for (const auto& s : expectedLines) {
    oss << s << endl;
  }
  string expectedJoined = oss.str();
  string actualOutput = output.str();

  EXPECT_TRUE(strWhitespaceEq(actualOutput, expectedJoined))
      << "See the test for the expected output. We ignore whitespace "
         "differences."
      << endl
      << "Your output:" << endl
      << actualOutput;
}

TEST_F(SearchEngine, cplusplus) {
  input << "vector" << endl;
  input << "vector +container" << endl;
  input << "vector +container +pointer" << endl;
  input << "vector +container -pointer" << endl;
  input << "vector bool ratio" << endl;
  input << "L +ratio" << endl;
  input << endl;
  input << endl;

  searchEngine("data/cplusplus.txt");

  vector<string> expectedLines = {
      // Info output
      "Stand by while building index...",
      "Indexed 86 pages containing 1498 unique terms",
      // Query "vector"
      "Enter query sentence (press enter to quit): Found 8 matching pages",
      "https://www.cplusplus.com/reference/array/array/",
      "https://www.cplusplus.com/reference/bitset/bitset/",
      "https://www.cplusplus.com/reference/forward_list/forward_list/",
      "https://www.cplusplus.com/reference/list/list/",
      "https://www.cplusplus.com/reference/queue/priority_queue/",
      "https://www.cplusplus.com/reference/stack/stack/",
      "https://www.cplusplus.com/reference/vector/vector-bool/",
      "https://www.cplusplus.com/reference/vector/vector/",
      // Query "vector +container"
      "Enter query sentence (press enter to quit): Found 7 matching pages",
      "https://www.cplusplus.com/reference/array/array/",
      "https://www.cplusplus.com/reference/forward_list/forward_list/",
      "https://www.cplusplus.com/reference/list/list/",
      "https://www.cplusplus.com/reference/queue/priority_queue/",
      "https://www.cplusplus.com/reference/stack/stack/",
      "https://www.cplusplus.com/reference/vector/vector-bool/",
      "https://www.cplusplus.com/reference/vector/vector/",
      // Query "vector +container +pointer"
      "Enter query sentence (press enter to quit): Found 1 matching pages",
      "https://www.cplusplus.com/reference/vector/vector-bool/",
      // Query "vector +container -pointer"
      "Enter query sentence (press enter to quit): Found 6 matching pages",
      "https://www.cplusplus.com/reference/array/array/",
      "https://www.cplusplus.com/reference/forward_list/forward_list/",
      "https://www.cplusplus.com/reference/list/list/",
      "https://www.cplusplus.com/reference/queue/priority_queue/",
      "https://www.cplusplus.com/reference/stack/stack/",
      "https://www.cplusplus.com/reference/vector/vector/",
      // Query "vector bool ratio"
      "Enter query sentence (press enter to quit): Found 24 matching pages",
      "https://www.cplusplus.com/is_fundamental",
      "https://www.cplusplus.com/reference/array/array/",
      "https://www.cplusplus.com/reference/bitset/bitset/",
      "https://www.cplusplus.com/reference/chrono/duration/",
      "https://www.cplusplus.com/reference/exception/exception_ptr/",
      "https://www.cplusplus.com/reference/forward_list/forward_list/",
      "https://www.cplusplus.com/reference/functional/binary_negate/",
      "https://www.cplusplus.com/reference/functional/unary_negate/",
      "https://www.cplusplus.com/reference/list/list/",
      "https://www.cplusplus.com/reference/queue/priority_queue/",
      "https://www.cplusplus.com/reference/ratio/ratio/",
      "https://www.cplusplus.com/reference/ratio/ratio_add/",
      "https://www.cplusplus.com/reference/ratio/ratio_divide/",
      "https://www.cplusplus.com/reference/ratio/ratio_greater/",
      "https://www.cplusplus.com/reference/ratio/ratio_greater_equal/",
      "https://www.cplusplus.com/reference/ratio/ratio_less/",
      "https://www.cplusplus.com/reference/ratio/ratio_less_equal/",
      "https://www.cplusplus.com/reference/ratio/ratio_multiply/",
      "https://www.cplusplus.com/reference/ratio/ratio_not_equal/",
      "https://www.cplusplus.com/reference/ratio/ratio_subtract/",
      "https://www.cplusplus.com/reference/regex/sub_match/",
      "https://www.cplusplus.com/reference/stack/stack/",
      "https://www.cplusplus.com/reference/vector/vector-bool/",
      "https://www.cplusplus.com/reference/vector/vector/",
      // Query "L +ratio"
      "Enter query sentence (press enter to quit): Found 0 matching pages",
      // Exit on empty query
      "Enter query sentence (press enter to quit): Thank you for searching!",
  };
  ostringstream oss;
  for (const auto& s : expectedLines) {
    oss << s << endl;
  }
  string expectedJoined = oss.str();
  string actualOutput = output.str();

  EXPECT_TRUE(strWhitespaceEq(actualOutput, expectedJoined))
      << "See the test for the expected output. We ignore whitespace "
         "differences."
      << endl
      << "Your output:" << endl
      << actualOutput;
}

TEST_F(SearchEngine, uiccs_news) {
  input << "build" << endl;
  input << "build +story" << endl;
  input << "build -story" << endl;
  input << "story -build" << endl;
  input << "story +build" << endl;
  input << "story +build grow" << endl;
  input << "story +build grow -google" << endl;
  input << "story +build grow -google -grow -story -build" << endl;
  input << endl;
  input << endl;

  searchEngine("data/uiccs-news.txt");

  // clang-format off
  vector<string> expectedLines = {
      // Info output
      "Stand by while building index...",
      "Indexed 85 pages containing 7032 unique terms",
      // Query "build"
      "Enter query sentence (press enter to quit): Found 16 matching pages",
      "https://cs.uic.edu/news-stories/andres-tapia-receives-scholarships-to-attend-tapia-conference-great-minds-in-stem-conference/",
      "https://cs.uic.edu/news-stories/computer-science-assistant-professor-chris-kanich-wins-graduate-mentoring-award/",
      "https://cs.uic.edu/news-stories/computer-science-students-earn-research-student-scholarships-to-attend-grace-hopper-celebration/",
      "https://cs.uic.edu/news-stories/cs-department-welcomes-three-new-faculty-members/",
      "https://cs.uic.edu/news-stories/cs-students-lead-edt-robot-team-to-semifinals-at-engineering-competition/",
      "https://cs.uic.edu/news-stories/cs-welcomes-13-new-faculty-members/",
      "https://cs.uic.edu/news-stories/engineering-students-learn-autonomous-vehicles-in-new-multidisciplinary-course/",
      "https://cs.uic.edu/news-stories/making-music-out-of-numbers-lessons-in-break-through-techs-cs-100-summer-course/",
      "https://cs.uic.edu/news-stories/meet-jovad-uribe-a-junior-in-uics-new-data-science-program/",
      "https://cs.uic.edu/news-stories/ms-students-win-mobility-hackathon-chicago-with-feel-your-city-platform/",
      "https://cs.uic.edu/news-stories/reaping-the-benefits-of-an-internship-experience/",
      "https://cs.uic.edu/news-stories/senior-dane-zieman-challenges-fellow-cs-students-to-prepare-for-technical-interviews/",
      "https://cs.uic.edu/news-stories/uic-aims-to-break-through-gender-gap-in-computer-science/",
      "https://cs.uic.edu/news-stories/uic-receives-explorecsr-grant-from-google/",
      "https://cs.uic.edu/news-stories/uic-receives-ongoing-funding-to-improve-diversity-in-computer-science/",
      "https://cs.uic.edu/news-stories/wearable-technology-course-expands-to-meet-demand/",
      // Query "build +story"
      "Enter query sentence (press enter to quit): Found 2 matching pages",
      "https://cs.uic.edu/news-stories/uic-aims-to-break-through-gender-gap-in-computer-science/",
      "https://cs.uic.edu/news-stories/wearable-technology-course-expands-to-meet-demand/",
      // Query "build -story"
      "Enter query sentence (press enter to quit): Found 14 matching pages",
      "https://cs.uic.edu/news-stories/andres-tapia-receives-scholarships-to-attend-tapia-conference-great-minds-in-stem-conference/",
      "https://cs.uic.edu/news-stories/computer-science-assistant-professor-chris-kanich-wins-graduate-mentoring-award/",
      "https://cs.uic.edu/news-stories/computer-science-students-earn-research-student-scholarships-to-attend-grace-hopper-celebration/",
      "https://cs.uic.edu/news-stories/cs-department-welcomes-three-new-faculty-members/",
      "https://cs.uic.edu/news-stories/cs-students-lead-edt-robot-team-to-semifinals-at-engineering-competition/",
      "https://cs.uic.edu/news-stories/cs-welcomes-13-new-faculty-members/",
      "https://cs.uic.edu/news-stories/engineering-students-learn-autonomous-vehicles-in-new-multidisciplinary-course/",
      "https://cs.uic.edu/news-stories/making-music-out-of-numbers-lessons-in-break-through-techs-cs-100-summer-course/",
      "https://cs.uic.edu/news-stories/meet-jovad-uribe-a-junior-in-uics-new-data-science-program/",
      "https://cs.uic.edu/news-stories/ms-students-win-mobility-hackathon-chicago-with-feel-your-city-platform/",
      "https://cs.uic.edu/news-stories/reaping-the-benefits-of-an-internship-experience/",
      "https://cs.uic.edu/news-stories/senior-dane-zieman-challenges-fellow-cs-students-to-prepare-for-technical-interviews/",
      "https://cs.uic.edu/news-stories/uic-receives-explorecsr-grant-from-google/",
      "https://cs.uic.edu/news-stories/uic-receives-ongoing-funding-to-improve-diversity-in-computer-science/",
      // Query "story -build"
      "Enter query sentence (press enter to quit): Found 9 matching pages",
      "https://cs.uic.edu/news-stories/biaffet-app-can-typos-give-insight-into-your-mental-health/",
      "https://cs.uic.edu/news-stories/board-grants-key-approvals-for-uics-new-health-care-engineering-buildings/",
      "https://cs.uic.edu/news-stories/cecilia-avila-uic-cs-20-wins-get-chicago-fellowship/",
      "https://cs.uic.edu/news-stories/computer-science-instructor-credits-course-success-to-online-learning/",
      "https://cs.uic.edu/news-stories/evl-to-be-featured-in-chicago-new-media-1973-1992-exhibition/",
      "https://cs.uic.edu/news-stories/inspiring-grads-discovering-passion-for-computer-science-pays-off/",
      "https://cs.uic.edu/news-stories/nsf-awards-uic-1-5m-for-new-data-science-institute/",
      "https://cs.uic.edu/news-stories/uics-electronic-visiualization-lab-is-helping-reimagine-live-stage-performance/",
      "https://cs.uic.edu/news-stories/university-scholar-barbara-di-eugenio/",
      // Query "story +build"
      "Enter query sentence (press enter to quit): Found 2 matching pages",
      "https://cs.uic.edu/news-stories/uic-aims-to-break-through-gender-gap-in-computer-science/",
      "https://cs.uic.edu/news-stories/wearable-technology-course-expands-to-meet-demand/",
      // Query "story +build grow"
      "Enter query sentence (press enter to quit): Found 11 matching pages",
      "https://cs.uic.edu/news-stories/board-grants-key-approvals-for-uics-new-health-care-engineering-buildings/",
      "https://cs.uic.edu/news-stories/break-through-tech-chicago-announces-strategic-expert-as-new-director/",
      "https://cs.uic.edu/news-stories/engineering-students-learn-autonomous-vehicles-in-new-multidisciplinary-course/",
      "https://cs.uic.edu/news-stories/foursquare-visits-uic-plans-to-expand-its-growth-in-chicago/",
      "https://cs.uic.edu/news-stories/googles-hispanic-leadership-summit-chicago-tailor-made-for-some-uic-computer-science-students/",
      "https://cs.uic.edu/news-stories/maxine-brown-director-of-uics-electronic-visualization-laboratory-retires-after-34-years/",
      "https://cs.uic.edu/news-stories/microsoft-ai-for-earth-adds-wild-me-to-50m-project-initiative/",
      "https://cs.uic.edu/news-stories/more-than-60-k-12-teachers-from-the-chicago-area-attended-cs-ed-camp-hosted-by-csta-and-the-uic-cs-department/",
      "https://cs.uic.edu/news-stories/uic-aims-to-break-through-gender-gap-in-computer-science/",
      "https://cs.uic.edu/news-stories/uic-college-of-engineering-researchers-selected-for-discovery-partners-institute-seed-funding/",
      "https://cs.uic.edu/news-stories/wearable-technology-course-expands-to-meet-demand/",
      // Query "story +build grow -google"
      "Enter query sentence (press enter to quit): Found 9 matching pages",
      "https://cs.uic.edu/news-stories/board-grants-key-approvals-for-uics-new-health-care-engineering-buildings/",
      "https://cs.uic.edu/news-stories/break-through-tech-chicago-announces-strategic-expert-as-new-director/",
      "https://cs.uic.edu/news-stories/engineering-students-learn-autonomous-vehicles-in-new-multidisciplinary-course/",
      "https://cs.uic.edu/news-stories/foursquare-visits-uic-plans-to-expand-its-growth-in-chicago/",
      "https://cs.uic.edu/news-stories/maxine-brown-director-of-uics-electronic-visualization-laboratory-retires-after-34-years/",
      "https://cs.uic.edu/news-stories/microsoft-ai-for-earth-adds-wild-me-to-50m-project-initiative/",
      "https://cs.uic.edu/news-stories/uic-aims-to-break-through-gender-gap-in-computer-science/",
      "https://cs.uic.edu/news-stories/uic-college-of-engineering-researchers-selected-for-discovery-partners-institute-seed-funding/",
      "https://cs.uic.edu/news-stories/wearable-technology-course-expands-to-meet-demand/",
      // Query "story +build grow -google -grow -story -build"
      "Enter query sentence (press enter to quit): Found 0 matching pages",
      // Exit on empty query
      "Enter query sentence (press enter to quit): Thank you for searching!",
  };
  // clang-format on
  ostringstream oss;
  for (const auto& s : expectedLines) {
    oss << s << endl;
  }
  string expectedJoined = oss.str();
  string actualOutput = output.str();

  EXPECT_TRUE(strWhitespaceEq(actualOutput, expectedJoined))
      << "See the test for the expected output. We ignore whitespace "
         "differences."
      << endl
      << "Your output:" << endl
      << actualOutput;
}

}  // namespace
