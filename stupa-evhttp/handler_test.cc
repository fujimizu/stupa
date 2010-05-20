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
#include "handler.h"

const size_t INV_SIZE = 50;
const size_t MAX_DOC  = 1000;

namespace {

};

/* add_document */
TEST(HandlerTest, AddDocumentTest) {
  stupa::evhttp::StupaSearchHandler handler(INV_SIZE, MAX_DOC);
}

/* delete_document */
TEST(HandlerTest, DeleteDocumentTest) {
}

/* size */
TEST(HandlerTest, SizeTest) {
}

/* clear */
TEST(HandlerTest, ClearTest) {
}

/* search_by_document */
TEST(HandlerTest, SearchByDocumentTest) {
}

/* search_by_feature */
TEST(HandlerTest, SearchByFeatureTest) {
}

/* save */
TEST(HandlerTest, SaveTest) {
}

/* load */
TEST(HandlerTest, LoadTest) {
}


int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
