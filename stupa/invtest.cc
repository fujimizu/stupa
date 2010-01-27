//
// Tests for inverted index class
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
#include "bayesian_sets.h"
#include "inverted_index.h"

namespace {

/* typedef */
typedef std::map<stupa::DocumentId, std::vector<stupa::FeatureId> > TestSet;
typedef std::map<stupa::FeatureId, std::vector<stupa::DocumentId> > Count;

/* constants */
const size_t NUM_DOC                  = 100;  ///< the number of documents
const size_t NUM_FEATURE              = 20;   ///< the number of feature
const stupa::FeatureId MAX_FEATURE_ID = 100;  ///< maximu feature id
const char *SAVE_FILE                 = "invtest_saved.tmp"; ///< save filename

/* function prototypes */
static void set_input_documents(TestSet &documents, Count &feature_count);
static void add_documents(stupa::InvertedIndex &inv, const TestSet &documents);
static void check_index(const stupa::InvertedIndex &inv,
                        const TestSet &documents, const Count &feature_count);

/* set test data */
static void set_input_documents(TestSet &documents, Count &feature_count) {
  for (size_t i = 0; i < NUM_DOC; i++) {
    stupa::DocumentId did = stupa::DOC_START_ID + i;
    std::vector<stupa::FeatureId> feature;
    std::map<stupa::FeatureId, bool> check;
    check[stupa::FEATURE_EMPTY_ID] = true;
    check[stupa::FEATURE_DELETED_ID] = true;
    size_t cnt = 0;
    while (cnt < NUM_FEATURE) {
      stupa::FeatureId fid = static_cast<stupa::FeatureId>(rand()) % MAX_FEATURE_ID;
      if (check.find(fid) == check.end()) {
        feature.push_back(fid);
        check[fid] = true;
        cnt++;

        Count::iterator it = feature_count.find(fid);
        if (it == feature_count.end()) {
          std::vector<stupa::DocumentId> v;
          feature_count[fid] = v;
        }
        feature_count[fid].push_back(did);
      }
    }
    documents[did] = feature;
  }
}

/* add documents to inverted indexes */
static void add_documents(stupa::InvertedIndex &inv, const TestSet &documents) {
  for (TestSet::const_iterator it = documents.begin();
       it != documents.end(); ++it) {
    inv.add_document(it->first, it->second);
  }
}

/* check inverted indexes */
static void check_index(const stupa::InvertedIndex &inv,
                        const TestSet &documents, const Count &feature_count) {
  EXPECT_EQ(feature_count.size(), inv.size());
  stupa::InvertedIndex::IndexHash index_hash = inv.index();
  for (stupa::InvertedIndex::IndexHash::const_iterator vit = index_hash.begin();
       vit != index_hash.end(); ++vit) {
    Count::const_iterator fit = feature_count.find(vit->first);
    EXPECT_TRUE(fit != feature_count.end());

    std::vector<stupa::DocumentId> document_ids;
    vit->second->list(document_ids);
    EXPECT_EQ(fit->second.size(), document_ids.size());
    EXPECT_TRUE(fit->second == document_ids);

    for (size_t i = 0; i < document_ids.size(); i++) {
      TestSet::const_iterator dit = documents.find(document_ids[i]);
      EXPECT_TRUE(dit != documents.end());
    }
  }
}

} /* namespace */

/* add_document */
TEST(InvertedIndexTest, AddDocumentTest) {
  TestSet documents;
  Count feature_count;
  set_input_documents(documents, feature_count);
  stupa::InvertedIndex inv;
  add_documents(inv, documents);

  check_index(inv, documents, feature_count);
}

/* delete_document */
TEST(InvertedIndexTest, DeleteDocumentTest) {
  TestSet documents;
  Count feature_count;
  set_input_documents(documents, feature_count);
  stupa::InvertedIndex inv;
  add_documents(inv, documents);

  // delete some documents
  size_t num_deleted = 0;
  for (TestSet::const_iterator it = documents.begin();
       it != documents.end(); ++it) {
    stupa::DocumentId did = it->first;
    if (did % 2 == 0) {
      inv.delete_document(did, it->second);
      num_deleted++;

      for (size_t i = 0; i < it->second.size(); i++) {
        Count::iterator fit = feature_count.find(it->second[i]);
        for (std::vector<stupa::DocumentId>::iterator vit = fit->second.begin();
             vit != fit->second.end(); vit++) {
          if (*vit == did) {
            fit->second.erase(vit);
            break;
          }
        }
      }
    }
  }

  check_index(inv, documents, feature_count);
}

/* clear */
TEST(InvertedIndexTest, ClearTest) {
  TestSet documents;
  Count feature_count;
  set_input_documents(documents, feature_count);
  stupa::InvertedIndex inv;
  add_documents(inv, documents);

  inv.clear();
  EXPECT_EQ(0, inv.size());
}

/* lookup */
TEST(InvertedIndexTest, LookupTest) {
  TestSet documents;
  Count feature_count;
  set_input_documents(documents, feature_count);
  stupa::InvertedIndex inv;
  add_documents(inv, documents);

  std::vector<stupa::FeatureId> features;
  std::map<stupa::DocumentId, bool> docmap;
  for (Count::iterator fit = feature_count.begin();
       fit != feature_count.end(); ++fit) {
    if (fit->first % 2 == 0) {
      features.push_back(fit->first);
      for (size_t i = 0; i < fit->second.size(); i++) {
        docmap[fit->second[i]] = true;
      }
    }
  }

  std::vector<stupa::DocumentId> results;
  inv.lookup(features, results);
  EXPECT_EQ(docmap.size(), results.size());
  for (size_t i = 0; i < results.size(); i++) {
    EXPECT_TRUE(docmap.find(results[i]) != docmap.end());
  }
}

/* lookup */
TEST(InvertedIndexTest, SaveLoadTest) {
  TestSet documents;
  Count feature_count;
  set_input_documents(documents, feature_count);
  stupa::InvertedIndex inv;
  add_documents(inv, documents);

  std::ofstream ofs(SAVE_FILE);
  inv.save(ofs);
  ofs.close();
  inv.clear();

  std::ifstream ifs(SAVE_FILE);
  inv.load(ifs);
  ifs.close();
  check_index(inv, documents, feature_count);
  remove(SAVE_FILE);
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
