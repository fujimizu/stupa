//
// Tests for Search Model class 
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
#include "search_model.h"

namespace {

typedef std::map<stupa::DocumentId, std::vector<stupa::FeatureId> > TestSet;
typedef std::map<stupa::FeatureId, size_t> Count;

const size_t NUM_DOC                  = 100;  //< the number of documents
const size_t NUM_FEATURE              = 20;   //< the number of feature
const size_t NUM_CANDIDATE            = 30;   //< the number of candidates
const stupa::FeatureId MAX_FEATURE_ID = 100;  //< maximum identifier of feature

template <typename Model>
class SearchModelTest {
 private:
  TestSet documents;
  Count features;

  /* set test data */
  void set_input_documents() {
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

  /* add documents to search model object */
  void add_documents(stupa::SearchModel &model, const TestSet &documents) {
    for (TestSet::const_iterator it = documents.begin();
         it != documents.end(); ++it) {
      model.add_document(it->first, it->second);
    }
  }

  /* check documents and features */
  void check_documents_and_features(
    const stupa::SearchModel &model, const TestSet &documents,
    const Count &features) {
    stupa::SearchModel::DocumentMap::const_iterator dit;
    for (dit = model.documents().begin(); dit != model.documents().end(); ++dit) {
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
      stupa::SearchModel::FeatureCount::const_iterator bit
        = model.feature_count().find(fit->first);
      EXPECT_TRUE(bit != model.feature_count().end());
      EXPECT_EQ(fit->second, bit->second);
    }
  }

  /* check search results */
  void check_search_results(
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

  /* clear */
  void clear_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

    model.clear();
    EXPECT_EQ(0, model.documents().size());
    EXPECT_EQ(0, model.feature_count().size());
  }

  /* add_document */
  void add_document_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

    EXPECT_EQ(NUM_DOC, model.size());
    check_documents_and_features(model, documents, features);
  }

  /* delete_document */
  void delete_document_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

    // delete some documents
    size_t num_deleted = 0;
    for (TestSet::const_iterator it = documents.begin();
         it != documents.end(); ++it) {
      if (it->first % 2 == 0) {
        model.delete_document(it->first);
        num_deleted++;
        for (size_t i = 0; i < it->second.size(); i++) {
          features[it->second[i]]--;
        }
      }
    }
    EXPECT_EQ(documents.size() - num_deleted, model.size());
    check_documents_and_features(model, documents, features);
  }

  /* update document */
  void update_document_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

    stupa::DocumentId id = stupa::DOC_START_ID;
    for (size_t i = 0; i < documents[id].size(); i++) {
      features[documents[id][i]]--;
    }
    for (size_t i = 0; i < documents[id+1].size(); i++) {
      features[documents[id+1][i]]++;
    }
    documents[id] = documents[id+1];
    model.add_document(id, documents[id+1]);  // update

    EXPECT_EQ(NUM_DOC, model.size());
    check_documents_and_features(model, documents, features);
  }

  /* search all documents */
  void search_all_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

    std::vector<stupa::DocumentId> queries;
    std::vector<std::pair<stupa::DocumentId, stupa::Point> > results;
    std::map<stupa::DocumentId, bool> candidates;
    for (TestSet::const_iterator it = documents.begin();
         it != documents.end(); ++it) {
      candidates[it->first] = true;
    }

    queries.push_back(stupa::DOC_START_ID);
    model.search_by_document(queries, results, model.size());
    check_search_results(candidates, results);  // size_of_queries == 1
    results.clear();
    queries.push_back(stupa::DOC_START_ID+1);
    queries.push_back(stupa::DOC_START_ID+2);
    model.search_by_document(queries, results, model.size());
    check_search_results(candidates, results);  // size_of_queries == 3
  }

  /* search some candidates */
  void search_candidates_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

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
    model.search_by_document(queries, candidate_ids, results, model.size());
    check_search_results(candidate_map, results);  // size_of_queries == 1
    results.clear();
    queries.push_back(stupa::DOC_START_ID+1);
    queries.push_back(stupa::DOC_START_ID+2);
    model.search_by_document(queries, candidate_ids, results, model.size());
    check_search_results(candidate_map, results);  // size_of_queries == 3
  }

  /* search same documents */
  void search_same_documents() {
    std::vector<stupa::FeatureId> features;
    for (size_t i = 0; i < NUM_FEATURE; i++) {
      features.push_back(stupa::FEATURE_START_ID + i);
    }
    Model model;
    for (size_t i = 0; i < NUM_DOC; i++) {
      model.add_document(stupa::DOC_START_ID + i, features);
    }
    std::vector<stupa::DocumentId> queries;
    std::vector<std::pair<stupa::DocumentId, stupa::Point> > results;
    queries.push_back(stupa::DOC_START_ID);
    model.search_by_document(queries, results, model.size());
    EXPECT_EQ(results.size(), NUM_DOC);
    stupa::Point point = results[0].second;
    for (size_t i = 1; i < results.size(); i++) {
      EXPECT_EQ(point, results[i].second);
    }
  }

  /* search some candidates */
  void search_limited_results_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

    std::vector<stupa::DocumentId> queries;
    queries.push_back(stupa::DOC_START_ID);
    std::vector<std::pair<stupa::DocumentId, stupa::Point> > all, half;
    model.search_by_document(queries, all, model.size());  // all documents
    model.search_by_document(queries, half, model.size() / 2);
    EXPECT_EQ(model.size() / 2, half.size());
    for (size_t i = 0; i < half.size(); i++) {
      EXPECT_TRUE(half[i] == all[i]);
    }
  }

  /* save/load */
  void save_load_test() {
    set_input_documents();
    Model model;
    add_documents(model, documents);

    const char filename[] = "modeltest_saved.tmp";
    std::ofstream ofs(filename);
    model.save(ofs);
    ofs.close();

    std::ifstream ifs(filename);
    model.clear();
    model.load(ifs);
    ifs.close();

    EXPECT_EQ(NUM_DOC, model.size());
    check_documents_and_features(model, documents, features);
    remove(filename);
  }

 public:
  void do_all_test() {
    clear_test();
    add_document_test();
    delete_document_test();
    update_document_test();
    search_all_test();
    search_candidates_test();
    search_limited_results_test();
    save_load_test();
  }
};

} /* namespace */

TEST(SearchModelTest, SearchModelInnerProduct) {
  SearchModelTest<stupa::SearchModelInnerProduct> test;
  test.do_all_test();
}
TEST(SearchModelTest, SearchModelCosineTest) {
  SearchModelTest<stupa::SearchModelCosine> test;
    test.do_all_test();
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
