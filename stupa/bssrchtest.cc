//
// Tests for BayesianSets searcher class
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
#include <map>
#include <vector>
#include "bssearch.h"
#include "util.h"

namespace {

/* typedef */
typedef std::map<std::string, std::vector<std::string> > TestSet;

/* constants */
const size_t NUM_DOC     = 100;  ///< the number of documents
const size_t NUM_FEATURE = 20;   ///< the number of features
const size_t STR_LENGTH  = 10;   ///< maximum length of feature strings
const char *SAVE_FILE    = "bssearchtest_saved.tmp";  ///< save filename

/* function prototypes */
static void set_input_documents(TestSet &documents);

/* set input documents */
static void set_input_documents(TestSet &documents) {
  std::map<std::string, bool> check_did;
  for (size_t i = 0; i < NUM_DOC; i++) {
    std::string did("");
    do {
      stupa::random_string(STR_LENGTH, did);
    } while (did.empty() || check_did.find(did) != check_did.end());
    check_did[did] = true;
    std::vector<std::string> feature;
    std::map<std::string, bool> check_fid;
    size_t cnt = 0;
    while (cnt < NUM_FEATURE) {
      std::string fid;
      stupa::random_string(STR_LENGTH, fid);
      if (check_fid.find(fid) == check_fid.end()) {
        feature.push_back(fid);
        check_fid[fid] = true;
        cnt++;
      }
    }
    documents[did] = feature;
  }
}

} /* namespace */

/* add_document */
TEST(BayesianSetsSearchTest, AddDocumentTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::BayesianSetsSearch bssearch;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    bssearch.add_document(it->first, it->second);

  }
  EXPECT_EQ(documents.size(), bssearch.size());

  int cnt = 0;
  size_t deleted = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (cnt++ % 2 == 0) {
      bssearch.delete_document(it->first);
      deleted++;
    }
  }
  EXPECT_EQ(documents.size() - deleted, bssearch.size());

  bssearch.clear();
  EXPECT_EQ(0, bssearch.size());
}

/* search */
TEST(BayesianSetsSearchTest, SearchTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::BayesianSetsSearch bssearch;
  std::vector<std::string> queries;
  int cnt = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (cnt++ % 3 == 0) queries.push_back(it->first);
    bssearch.add_document(it->first, it->second);
  }

  std::vector<std::pair<std::string, stupa::Point> > results;
  bssearch.search(queries, results);
  EXPECT_TRUE(results.size() > 0);
  for (size_t i = 0; i < results.size(); i++) {
    EXPECT_TRUE(documents.find(results[i].first) != documents.end());
    EXPECT_TRUE(results[i].second > 0);
  }
}

/* save/load */
TEST(BayesianSetsSearchTest, SaveLoadTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::BayesianSetsSearch bssearch;
  std::vector<std::string> queries;
  int cnt = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (cnt++ % 3 == 0) queries.push_back(it->first);
    bssearch.add_document(it->first, it->second);
  }
  std::vector<std::pair<std::string, stupa::Point> > results;
  bssearch.search(queries, results);

  std::ofstream ofs(SAVE_FILE);
  bssearch.save(ofs);
  ofs.close();
  bssearch.clear();

  std::ifstream ifs(SAVE_FILE);
  bssearch.load(ifs);
  ifs.close();
  EXPECT_EQ(documents.size(), bssearch.size());

  std::vector<std::pair<std::string, stupa::Point> > results_loaded;
  bssearch.search(queries, results_loaded);
  EXPECT_EQ(results.size(), results_loaded.size());
  for (size_t i = 0; i < results.size(); i++) {
    EXPECT_EQ(results[i].first, results_loaded[i].first);
    EXPECT_EQ(results[i].second, results_loaded[i].second);
  }
  
  remove(SAVE_FILE);
}

int main(int argc, char **argv) {
  unsigned int t = time(NULL);
  t = 1259062488;
  srand(t);
  //srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
