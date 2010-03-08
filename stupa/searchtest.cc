//
// Tests for Stupa Search class
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
#include "search.h"
#include "util.h"

namespace {

/* typedef */
typedef std::map<std::string, std::vector<std::string> > TestSet;

/* constants */
const size_t NUM_DOC     = 100;  ///< the number of documents
const size_t NUM_FEATURE = 20;   ///< the number of features
const size_t STR_LENGTH  = 10;   ///< maximum length of feature strings
const char *SAVE_FILE    = "searchtest_saved.tmp";  ///< save filename

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
    size_t count = 0;
    while (count < NUM_FEATURE) {
      std::string fid;
      stupa::random_string(STR_LENGTH, fid);
      if (check_fid.find(fid) == check_fid.end()) {
        feature.push_back(fid);
        check_fid[fid] = true;
        count++;
      }
    }
    documents[did] = feature;
  }
}

} /* namespace */

/* add_document */
TEST(StupaSearchTest, AddDocumentTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::StupaSearch stpsearch;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    stpsearch.add_document(it->first, it->second);
  }
  EXPECT_EQ(documents.size(), stpsearch.size());

  int count = 0;
  size_t deleted = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (count++ % 2 == 0) {
      stpsearch.delete_document(it->first);
      deleted++;
    }
  }
  EXPECT_EQ(documents.size() - deleted, stpsearch.size());

  stpsearch.clear();
  EXPECT_EQ(0, stpsearch.size());
}

TEST(StupaSearchTest, AddDocumentLimitTest) {
  TestSet documents;
  set_input_documents(documents);
  size_t invsize = 100;
  size_t max_doc = documents.size() / 2;
  stupa::StupaSearch stpsearch(stupa::SearchModel::INNER_PRODUCT,
                               invsize, max_doc);
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    stpsearch.add_document(it->first, it->second);
  }
  EXPECT_EQ(max_doc, stpsearch.size());

  std::vector<std::string> queries;
  std::vector<std::pair<std::string, stupa::Point> > results;
  size_t count = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    queries.push_back(it->first);
    stpsearch.search_by_document(queries, results);
    if (count < max_doc) {
      EXPECT_EQ(0, results.size());
    } else {
      EXPECT_LT(0, results.size());
    }
    queries.clear();
    results.clear();
    count++;
  }
}

/* search_by_document */
TEST(StupaSearchTest, SearchByDocumentTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::StupaSearch stpsearch;
  std::vector<std::string> queries;
  int count = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (count++ % 3 == 0) queries.push_back(it->first);
    stpsearch.add_document(it->first, it->second);
  }

  std::vector<std::pair<std::string, stupa::Point> > results;
  stpsearch.search_by_document(queries, results);
  EXPECT_LT(0, results.size());
  for (size_t i = 0; i < results.size(); i++) {
    EXPECT_TRUE(documents.find(results[i].first) != documents.end());
    EXPECT_LT(0, results[i].second);
  }
}

/* search_by_feature */
TEST(StupaSearchTest, SearchByFeatureTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::StupaSearch stpsearch;
  std::vector<std::string> queries;
  int count = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (count++ % 3 == 0) queries.push_back(it->second.at(0));
    stpsearch.add_document(it->first, it->second);
  }

  std::vector<std::pair<std::string, stupa::Point> > results;
  stpsearch.search_by_feature(queries, results);
  EXPECT_LT(0, results.size());
  for (size_t i = 0; i < results.size(); i++) {
    EXPECT_TRUE(documents.find(results[i].first) != documents.end());
    EXPECT_LT(0, results[i].second);
  }
}

/* save, load */
TEST(StupaSearchTest, SaveLoadTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::StupaSearch stpsearch;
  std::vector<std::string> queries;
  int count = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (count++ % 3 == 0) queries.push_back(it->first);
    stpsearch.add_document(it->first, it->second);
  }
  std::vector<std::pair<std::string, stupa::Point> > results;
  stpsearch.search_by_document(queries, results);

  std::ofstream ofs(SAVE_FILE);
  stpsearch.save(ofs);
  ofs.close();
  stpsearch.clear();

  std::ifstream ifs(SAVE_FILE);
  stpsearch.load(ifs);
  ifs.close();
  EXPECT_EQ(documents.size(), stpsearch.size());

  std::vector<std::pair<std::string, stupa::Point> > results_loaded;
  stpsearch.search_by_document(queries, results_loaded);
  EXPECT_EQ(results.size(), results_loaded.size());
  for (size_t i = 0; i < results.size(); i++) {
    EXPECT_EQ(results[i].first, results_loaded[i].first);
    EXPECT_EQ(results[i].second, results_loaded[i].second);
  }

  remove(SAVE_FILE);
}

/* save, load */
TEST(StupaSearchTest, SaveLoadLimitTest) {
  TestSet documents;
  set_input_documents(documents);
  stupa::StupaSearch stpsearch;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    stpsearch.add_document(it->first, it->second);
  }
  std::ofstream ofs(SAVE_FILE);
  stpsearch.save(ofs);
  ofs.close();

  size_t invsize = 100;
  size_t max_doc = documents.size() / 2;
  stupa::StupaSearch stpsearch_lim(stupa::SearchModel::INNER_PRODUCT,
                                   invsize, max_doc);
  std::ifstream ifs(SAVE_FILE);
  stpsearch_lim.load(ifs);
  ifs.close();
  EXPECT_EQ(max_doc, stpsearch_lim.size());

  std::vector<std::string> queries;
  std::vector<std::pair<std::string, stupa::Point> > results;
  size_t count = 0;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    queries.push_back(it->first);
    stpsearch_lim.search_by_document(queries, results);
    if (count < max_doc) {
      EXPECT_EQ(0, results.size());
    } else {
      EXPECT_LT(0, results.size());
    }
    queries.clear();
    results.clear();
    count++;
  }

  remove(SAVE_FILE);
}

int main(int argc, char **argv) {
  unsigned int t = time(NULL);
  t = 1259062488;
  srand(t);
  // srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
