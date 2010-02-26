//
// Tests for Bayesian Sets library
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
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <map>
#include <vector>
#include "bayesian_sets.h"

namespace {

/* typedef */
typedef std::map<stupa::DocumentId, std::vector<stupa::FeatureId> > TestSet;
typedef std::map<stupa::FeatureId, size_t> Count;

/* constants */
const size_t NUM_DOC                  = 100;  ///< the number of documents
const size_t NUM_FEATURE              = 20;   ///< the number of feature
const size_t NUM_CANDIDATE            = 30;   ///< the number of candidates
const stupa::FeatureId MAX_FEATURE_ID = 100;  ///< maximum identifier of feature

/* function prototypes */
static void set_input_documents();
static void add_documents(stupa::BayesianSets &bs, const TestSet &documents);
static void check_documents_and_features(const stupa::BayesianSets &bs,
                                         const TestSet &documents,
                                         const Count &features);
static void check_search_results(
  const std::map<stupa::DocumentId, bool> &candidates,
  const std::vector<std::pair<stupa::DocumentId, stupa::Point> > &results);

/* global variables */
TestSet documents;
Count features;

/* set test data */
static void set_input_documents() {
  documents.clear();
  features.clear();
  for (size_t i = 0; i < NUM_DOC; i++) {
    stupa::DocumentId id = stupa::DOC_START_ID + i;
    std::vector<stupa::FeatureId> feature;
    std::map<stupa::FeatureId, bool> check;
    check[stupa::FEATURE_EMPTY_ID] = true;
    check[stupa::FEATURE_DELETED_ID] = true;
    size_t cnt = 0;
    while (cnt < NUM_FEATURE) {
      stupa::FeatureId fid =
        static_cast<stupa::FeatureId>(rand()) % MAX_FEATURE_ID;
      if (check.find(fid) == check.end()) {
        feature.push_back(fid);
        check[fid] = true;
        cnt++;
        Count::iterator it = features.find(fid);
        features[fid] = (it != features.end()) ? it->second + 1 : 1;
      }
    }
    documents[id] = feature;
  }
}

/* add documents to bayesian sets */
static void add_documents(stupa::BayesianSets &bs, const TestSet &documents) {
  for (TestSet::const_iterator it = documents.begin();
       it != documents.end(); ++it) {
    bs.add_document(it->first, it->second);
  }
}

/* check documents and features */
static void check_documents_and_features(
  const stupa::BayesianSets &bs, const TestSet &documents,
  const Count &features) {
  stupa::BayesianSets::DocumentMap::const_iterator dit;
  for (dit = bs.documents().begin(); dit != bs.documents().end(); ++dit) {
    // check added documents
    TestSet::const_iterator tit = documents.find(dit->first);
    EXPECT_TRUE(tit != documents.end());
    std::vector<stupa::FeatureId> feature_ids;
    stupa::decompress_diff(dit->second, feature_ids);
    EXPECT_EQ(tit->second.size(), feature_ids.size());
    for (size_t i = 0; i < tit->second.size(); i++) {
      EXPECT_EQ(tit->second[i], feature_ids[i]);
    }
  }
  // check feature count
  for (Count::const_iterator fit = features.begin();
       fit != features.end(); ++fit) {
    stupa::BayesianSets::FeatureCount::const_iterator bit
      = bs.feature_count().find(fit->first);
    EXPECT_TRUE(bit != bs.feature_count().end());
    EXPECT_EQ(fit->second, bit->second);
  }
}

/* check search results */
static void check_search_results(
  const std::map<stupa::DocumentId, bool> &candidates,
  const std::vector<std::pair<stupa::DocumentId, stupa::Point> > &results) {
  EXPECT_LT(0, results.size());
  EXPECT_LE(results.size(), candidates.size());
  stupa::Point prev_score = 0;
  for (size_t i = 0; i < results.size(); i++) {
    if (i > 0) EXPECT_TRUE(results[i].second <= prev_score);
    EXPECT_TRUE(candidates.find(results[i].first) != candidates.end());
    prev_score = results[i].second;
  }
}

} /* namespace */

/* clear */
TEST(BayesianSetsTest, ClearTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  bs.clear();
  EXPECT_EQ(0, bs.documents().size());
  EXPECT_EQ(0, bs.feature_count().size());
}

/* add_document */
TEST(BayesianSetsTest, AddDocumentTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  EXPECT_EQ(NUM_DOC, bs.size());
  check_documents_and_features(bs, documents, features);
}

/* delete_document */
TEST(BayesianSetsTest, DeleteDocumentTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  // delete some documents
  size_t num_deleted = 0;
  for (TestSet::const_iterator it = documents.begin();
       it != documents.end(); ++it) {
    if (it->first % 2 == 0) {
      bs.delete_document(it->first);
      num_deleted++;
      for (size_t i = 0; i < it->second.size(); i++) {
        features[it->second[i]]--;
      }
    }
  }
  EXPECT_EQ(documents.size() - num_deleted, bs.size());
  check_documents_and_features(bs, documents, features);
}

/* update document */
TEST(BayesianSetsTest, UpdateDocumentTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  stupa::DocumentId id = stupa::DOC_START_ID;
  for (size_t i = 0; i < documents[id].size(); i++) {
    features[documents[id][i]]--;
  }
  for (size_t i = 0; i < documents[id+1].size(); i++) {
    features[documents[id+1][i]]++;
  }
  documents[id] = documents[id+1];
  bs.add_document(id, documents[id+1]);  // update

  EXPECT_EQ(NUM_DOC, bs.size());
  check_documents_and_features(bs, documents, features);
}

/* search all documents */
TEST(BayesianSetsTest, SearchAllTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  std::vector<stupa::DocumentId> queries;
  std::vector<std::pair<stupa::DocumentId, stupa::Point> > results;
  std::map<stupa::DocumentId, bool> candidates;
  for (TestSet::const_iterator it = documents.begin();
       it != documents.end(); ++it) {
    candidates[it->first] = true;
  }

  queries.push_back(stupa::DOC_START_ID);
  bs.search_by_document(queries, results, bs.size());
  check_search_results(candidates, results);  // size_of_queries == 1
  results.clear();
  queries.push_back(stupa::DOC_START_ID+1);
  queries.push_back(stupa::DOC_START_ID+2);
  bs.search_by_document(queries, results, bs.size());
  check_search_results(candidates, results);  // size_of_queries == 3
}

/* search some candidates */
TEST(BayesianSetsTest, SearchCandidatesTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  std::vector<stupa::DocumentId> queries;
  std::vector<stupa::DocumentId> candidate_ids;
  std::map<stupa::DocumentId, bool> candidate_map;
  std::vector<std::pair<stupa::DocumentId, stupa::Point> > results;

  // select candidates
  size_t cnt = 0;
  while (cnt < NUM_CANDIDATE) {
    stupa::DocumentId id = static_cast<stupa::DocumentId>(rand()) % NUM_DOC;
    if (candidate_map.find(id) == candidate_map.end()) {
      candidate_ids.push_back(id);
      candidate_map[id] = true;
      cnt++;
    }
  }

  queries.push_back(stupa::DOC_START_ID);
  bs.search_by_document(queries, candidate_ids, results, bs.size());
  check_search_results(candidate_map, results);  // size_of_queries == 1
  results.clear();
  queries.push_back(stupa::DOC_START_ID+1);
  queries.push_back(stupa::DOC_START_ID+2);
  bs.search_by_document(queries, candidate_ids, results, bs.size());
  check_search_results(candidate_map, results);  // size_of_queries == 3
}

/* search same documents */
TEST(BayesianSetsTest, SearchSameDocumentsTest) {
  std::vector<stupa::FeatureId> features;
  for (size_t i = 0; i < NUM_FEATURE; i++) {
    features.push_back(stupa::FEATURE_START_ID + i);
  }
  stupa::BayesianSets bs;
  for (size_t i = 0; i < NUM_DOC; i++) {
    bs.add_document(stupa::DOC_START_ID + i, features);
  }
  std::vector<stupa::DocumentId> queries;
  std::vector<std::pair<stupa::DocumentId, stupa::Point> > results;
  queries.push_back(stupa::DOC_START_ID);
  bs.search_by_document(queries, results, bs.size());
  EXPECT_EQ(results.size(), NUM_DOC);
  stupa::Point point = results[0].second;
  for (size_t i = 1; i < results.size(); i++) {
    EXPECT_EQ(point, results[i].second);
  }
}

/* search some candidates */
TEST(BayesianSetsTest, SearchLimitedResultsTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  std::vector<stupa::DocumentId> queries;
  queries.push_back(stupa::DOC_START_ID);
  std::vector<std::pair<stupa::DocumentId, stupa::Point> > all, half;
  bs.search_by_document(queries, all, bs.size());  // all documents
  bs.search_by_document(queries, half, bs.size() / 2);
  EXPECT_EQ(bs.size() / 2, half.size());
  for (size_t i = 0; i < half.size(); i++) {
    EXPECT_TRUE(half[i] == all[i]);
  }
}

/* save/load */
TEST(BayesianSetsTest, SaveLoadTest) {
  set_input_documents();
  stupa::BayesianSets bs;
  add_documents(bs, documents);

  const char filename[] = "bstest_saved.tmp";
  std::ofstream ofs(filename);
  bs.save(ofs);
  ofs.close();

  std::ifstream ifs(filename);
  bs.clear();
  bs.load(ifs);
  ifs.close();

  EXPECT_EQ(NUM_DOC, bs.size());
  check_documents_and_features(bs, documents, features);
  remove(filename);
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
