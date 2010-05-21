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
#include <cstdlib>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "handler.h"

namespace {

/* typedef */
typedef std::map<std::string, std::vector<std::string> > TestSet;

/* constants */
const size_t INV_SIZE    = 50;    ///< the maximum size of inverted indexes
const size_t MAX_DOC     = 1000;  ///< the maximum number of documents
const size_t MAX_RESULT  = 100;   ///< the maximum number of search results
const size_t NUM_DOC     = 100;   ///< the number of documents in test sets
const size_t NUM_FEATURE = 20;    ///< the number of feature in test sets
const size_t LEN_ID      = 10;
const char *SAVE_FILE    = "handlertest_%d.tmp";  ///< save filename

/* generate a randomized string */
static std::string random_string(size_t length) {
  const std::string CHARACTERS
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
  std::stringstream ss;
  for (size_t i = 0; i < length; i++) {
    ss << CHARACTERS.at(rand() % CHARACTERS.size());
  }
  return ss.str();
}

/* set test data */
static void set_input_documents(TestSet &documents) {
  for (size_t i = 0; i < NUM_DOC; i++) {
    std::string document_id = random_string(LEN_ID);
    std::vector<std::string> features;
    for (size_t j = 0; j < NUM_FEATURE; j++) {
      features.push_back(random_string(LEN_ID));
    }
    documents[document_id] = features;
  }
}

/* add documents to a search handler */
static void add_documents(stupa::evhttp::StupaSearchHandler &handler,
                          const TestSet &documents) {
  for (TestSet::const_iterator it = documents.begin();
       it != documents.end(); ++it) {
    handler.add_document(it->first, it->second);
  }
}

} /* namespace */

/* size */
TEST(HandlerTest, SizeTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
  EXPECT_EQ(0, handler.size());
}

/* add_document */
TEST(HandlerTest, AddDocumentTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
  TestSet documents;
  set_input_documents(documents);
  add_documents(handler, documents);
  EXPECT_EQ(documents.size(), handler.size());
}

/* delete_document */
TEST(HandlerTest, DeleteDocumentTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
  TestSet documents;
  set_input_documents(documents);
  add_documents(handler, documents);

  size_t count = 0;
  std::vector<std::string> deleted_documents;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    if (count++ % 3 == 0) {
      handler.delete_document(it->first);
      deleted_documents.push_back(it->first);
    }
  }
  EXPECT_EQ(documents.size() - deleted_documents.size(), handler.size());

  std::vector<std::string> query;
  std::vector<std::pair<std::string, double> > results;
  for (size_t i = 0; i < deleted_documents.size(); i++) {
    query.clear();
    results.clear();
    query.push_back(deleted_documents[i]);
    handler.search_by_document(query, results, MAX_RESULT);
    EXPECT_EQ(0, results.size());
  }
}

/* clear */
TEST(HandlerTest, ClearTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
  TestSet documents;
  set_input_documents(documents);
  add_documents(handler, documents);

  EXPECT_LT(0, handler.size());
  handler.clear();
  EXPECT_EQ(0, handler.size());
}

/* search_by_document */
TEST(HandlerTest, SearchByDocumentTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
  TestSet documents;
  set_input_documents(documents);
  add_documents(handler, documents);

  std::vector<std::string> query;
  std::vector<std::pair<std::string, double> > results;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    query.clear();
    results.clear();
    query.push_back(it->first);
    handler.search_by_document(query, results, MAX_RESULT);
    EXPECT_LT(0, results.size());
  }
}

/* search_by_feature */
TEST(HandlerTest, SearchByFeatureTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
  TestSet documents;
  set_input_documents(documents);
  add_documents(handler, documents);

  std::vector<std::pair<std::string, double> > results;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    results.clear();
    handler.search_by_feature(it->second, results, MAX_RESULT);
    EXPECT_LT(0, results.size());
  }
}

/* save, load */
TEST(HandlerTest, SaveLoadTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
  TestSet documents;
  set_input_documents(documents);
  add_documents(handler, documents);

  char filename[128];
  sprintf(filename, SAVE_FILE, time(NULL));
  bool ret = handler.save(filename);
  ASSERT_TRUE(ret);

  handler.clear();
  EXPECT_EQ(0, handler.size());
  ret = handler.load(filename);
  ASSERT_TRUE(ret);
  EXPECT_EQ(documents.size(), handler.size());

  // search by document id
  std::vector<std::string> query;
  std::vector<std::pair<std::string, double> > results;
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    query.clear();
    results.clear();
    query.push_back(it->first);
    handler.search_by_document(query, results, MAX_RESULT);
    EXPECT_LT(0, results.size());
  }

  // search by feature id
  for (TestSet::iterator it = documents.begin(); it != documents.end(); ++it) {
    results.clear();
    handler.search_by_feature(it->second, results, MAX_RESULT);
    EXPECT_LT(0, results.size());
  }
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
