//
// Tests for Utility functions
//
// Copyright(C) 2010  Mizuki Fujisawa <fujisawa@bayon.cc>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include <cmath>
#include <ctime>
#include <gtest/gtest.h>
#include <map>
#include <utility>
#include <vector>
#include "util.h"

namespace {

/* constants */
const size_t NUM_PAIRS     = 100;    ///< number of pairs
const size_t NUM_INTEGERS  = 10;     ///< number of integers
const uint64_t MAX_INTEGER = 100000; ///< maximum integer

/* function prototypes */
static void random_pairs(size_t size,
                         std::vector<std::pair<int, double> > &pairs);
static void random_integers(size_t size, std::vector<uint64_t> &v);

/* set random pairs */
static void random_pairs(size_t size,
                         std::vector<std::pair<int, double> > &pairs) {
  for (size_t i = 0; i < size; i++) {
    std::pair<int, double> p;
    p.first = i;
    p.second = rand();
    if (i % 2 == 0) p.second *= -1;
    pairs.push_back(p);
  }
}

/* set random integers */
static void random_integers(size_t size, std::vector<uint64_t> &v) {
  std::map<uint64_t, bool> check;
  size_t cnt = 0;
  while (cnt < size) {
    uint64_t r = rand();
    // uint64_t r = rand() % MAX_INTEGER;
    if (r > 0 && check.find(r) == check.end()) {
      v.push_back(r);
      check[r] = true;
      cnt++;
    }
  }
  std::sort(v.begin(), v.end());
}

} /* namespace */

/* greater_pair */
TEST(UtilTest, ComparePairItemsTest) {
  std::vector<std::pair<int, double> > pairs;
  random_pairs(NUM_PAIRS, pairs);
  std::sort(pairs.begin(), pairs.end(), stupa::greater_pair<int, double>);
  for (size_t i = 1; i < pairs.size(); i++) {
    EXPECT_TRUE(pairs[i-1].second >= pairs[i].second);
  }
}

/* greater_pair */
TEST(UtilTest, ComparePairItemsSameKeyTest) {
  std::vector<std::pair<int, int> > pairs;
  pairs.push_back(std::pair<int, int>(1, 1));
  pairs.push_back(std::pair<int, int>(2, 1));
  std::sort(pairs.begin(), pairs.end(), stupa::greater_pair<int, int>);
  EXPECT_TRUE(pairs[0].first > pairs[1].first);
}

/* greater_pair_abs */
TEST(UtilTest, ComparePairItemsAbsTest) {
  std::vector<std::pair<int, double> > pairs;
  random_pairs(NUM_PAIRS, pairs);
  std::sort(pairs.begin(), pairs.end(), stupa::greater_pair_abs<int, double>);
  for (size_t i = 1; i < pairs.size(); i++) {
    EXPECT_TRUE(std::abs(pairs[i-1].second) >= std::abs(pairs[i].second));
  }
}

/* greater_pair_abs */
TEST(UtilTest, ComparePairItemsAbsSameKeyTest) {
  std::vector<std::pair<int, int> > pairs;
  pairs.push_back(std::pair<int, int>(1, 1));
  pairs.push_back(std::pair<int, int>(2, -1));
  std::sort(pairs.begin(), pairs.end(), stupa::greater_pair_abs<int, int>);
  EXPECT_TRUE(pairs[0].first > pairs[1].first);
}

/* compress_diff */
TEST(UtilTest, CompressDiffTest) {
  std::vector<uint64_t> input;
  random_integers(NUM_INTEGERS, input);
  char *enc = stupa::compress_diff(input);
  std::vector<uint64_t> output;
  stupa::decompress_diff(enc, output);
  EXPECT_TRUE(input == output);
  delete [] enc;
}

/* compress_diff */
TEST(UtilTest, CompressDiffPowerNumTest) {
  std::vector<uint64_t> input;
  uint64_t prev = 0;
  for (int i = 0; i < 5; i++) {
    input.push_back(prev + static_cast<uint64_t>(::pow(128, i)));
    prev = input[i];
  }
  char *enc = stupa::compress_diff(input);
  std::vector<uint64_t> output;
  stupa::decompress_diff(enc, output);
  EXPECT_TRUE(input == output);
  delete [] enc;
}

/* compress_diff with iterator inputs */
TEST(UtilTest, CompressDiffIterator) {
  std::vector<uint64_t> input;
  random_integers(NUM_INTEGERS, input);
  char *enc = stupa::compress_diff(input.begin(), input.end());
  std::vector<uint64_t> output;
  stupa::decompress_diff(enc, output);
  EXPECT_TRUE(input == output);
  delete [] enc;
}

/* get_extention */
TEST(UtilTest, GetExtensionTest) {
  std::string filename;
  filename = "test.tsv";
  EXPECT_EQ("tsv", stupa::get_extension(filename));

  filename = "test.tsv.txt";
  EXPECT_EQ("txt", stupa::get_extension(filename));

  filename = "test.tsv.";
  EXPECT_EQ("", stupa::get_extension(filename));

  filename = "test";
  EXPECT_EQ("", stupa::get_extension(filename));
}

/* split_string */
TEST(UtilTest, SplitStringTest) {
  std::string input;
  std::vector<std::string> splited;

  // space delimiter
  input = "This is a pen";
  stupa::split_string(input, " ", splited);
  EXPECT_EQ(4, splited.size());
  EXPECT_EQ("This", splited[0]);
  EXPECT_EQ("is",   splited[1]);
  EXPECT_EQ("a",    splited[2]);
  EXPECT_EQ("pen",  splited[3]);
  splited.clear();

  // tab delimiter
  input = "This	is	a	pen";
  stupa::split_string(input, "\t", splited);
  //stupa::split_string(input, "	", splited);
  EXPECT_EQ(4, splited.size());
  EXPECT_EQ("This", splited[0]);
  EXPECT_EQ("is",   splited[1]);
  EXPECT_EQ("a",    splited[2]);
  EXPECT_EQ("pen",  splited[3]);
  splited.clear();

  // space delimiter
  input = "あい うえ おか";
  stupa::split_string(input, " ", splited);
  EXPECT_EQ(3, splited.size());
  EXPECT_EQ("あい", splited[0]);
  EXPECT_EQ("うえ",   splited[1]);
  EXPECT_EQ("おか",    splited[2]);
  splited.clear();

  // tab delimiter
  input = "あい	うえ	おか";
  stupa::split_string(input, "\t", splited);
  EXPECT_EQ(3, splited.size());
  EXPECT_EQ("あい", splited[0]);
  EXPECT_EQ("うえ",   splited[1]);
  EXPECT_EQ("おか",    splited[2]);
  splited.clear();
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
