//
// Tests for posting list class
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

#include <gtest/gtest.h>
#include <fstream>
#include <map>
#include <vector>
#include "posting_list.h"

namespace {

/* constants */
const char *SAVE_FILE = "postest_saved.tmp";

/* function prototypes */
static void random_integers(size_t size, std::vector<uint64_t> &v);
template <typename PostingList>
static void do_tests(PostingList &plist);

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

/* do test */
template <typename PostingList>
static void do_tests(PostingList &plist) {
  const size_t size = 1000;
  std::vector<uint64_t> input, v;
  random_integers(size, input);

  // add
  for (size_t i = 0; i < input.size(); i++) {
    plist.add(input[i]);
  }
  plist.list(v);
  EXPECT_TRUE(v == input);

  // clear
  plist.clear();
  EXPECT_TRUE(plist.empty());
  v.clear();
  plist.list(v);
  EXPECT_TRUE(v.empty());

  // add with max size
  for (size_t i = 0; i < input.size(); i++) {
    plist.add(input[i], size / 2);
  }
  v.clear();
  plist.list(v);
  EXPECT_EQ(size / 2, v.size());
  for (size_t i = 0; i < v.size(); i++) {
    EXPECT_EQ(input[i + size/2], v[i]);
  }

  // remove
  size_t cnt = 0;
  std::vector<uint64_t> remain;
  for (size_t i = 0; i < v.size(); i++) {
    if (i % 2 == 0) {
      plist.remove(v[i]);
      cnt++;
    } else {
      remain.push_back(v[i]);
    }
  }
  v.clear();
  plist.list(v);
  EXPECT_TRUE(remain.size() == v.size());

  // save, load
  plist.clear();
  for (size_t i = 0; i < input.size(); i++) {
    plist.add(input[i]);
  }
  std::ofstream ofs(SAVE_FILE);
  plist.save(ofs);
  ofs.close();
  plist.clear();

  std::ifstream ifs(SAVE_FILE);
  plist.load(ifs);
  v.clear();
  plist.list(v);
  EXPECT_TRUE(v == input);
  ifs.close();
  remove(SAVE_FILE);
}

} /* namespace */

/* test for VectorPostingList class */
TEST(PostingListTest, VectorPostingListTest) {
  stupa::VectorPostingList plist;
  do_tests(plist);
}

/* test for VarBytePostingList class */
TEST(PostingListTest, VarBytePostingListTest) {
  stupa::VarBytePostingList plist;
  do_tests(plist);
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
