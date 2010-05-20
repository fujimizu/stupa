//
// Event handler
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

#ifndef STUPA_HANDLER_H_
#define STUPA_HANDLER_H_

#include <cstring>
#include "thread.h"
#include "stupa.h"

namespace stupa { namespace evhttp { /* namespace stupa::evhttp */

class StupaSearchHandler {
 private:
  StupaSearch stpsearch_; ///<  stupa search
  ReadWriteLock lock_;

 public:
  /**
   * Constructor.
   * @param invsize maximum size of inverted indexes
   * @param max_doc maximum number of documents
   */
  StupaSearchHandler(size_t invsize, size_t max_doc)
    : stpsearch_(SearchModel::INNER_PRODUCT,
                 invsize, max_doc) { }

  /**
   * Add a document.
   * @param document_id the identifier of input document
   * @param features feature strings of input document
   */
  void add_document(const std::string &document_id,
                    const std::vector<std::string> &features) {
    if (document_id.empty() || features.empty()) return;
    RWGuard m(lock_, true);
    stpsearch_.add_document(document_id, features);
  }

  /**
   * Delete a document.
   * @param document_id the identifier of document to be deleted
   */
  void delete_document(const std::string &document_id) {
    if (document_id.empty()) return;
    RWGuard m(lock_, true);
    stpsearch_.delete_document(document_id);
  }

  /**
   * Get the number of documents
   * @return the number of documents
   */
  int64_t size() {
    RWGuard m(lock_, false);
    return static_cast<uint64_t>(stpsearch_.size());
  }

  /**
   * Clear status.
   * @param filename file name
   */
  void clear() {
    RWGuard m(lock_, true);
    stpsearch_.clear();
  }

  /**
   * Search related documents using queries of document ids.
   * @param query the identifiers of query documents
   * @param result search result
   * @param max maximum number of output documents
   */
  void search_by_document(
    const std::vector<std::string> & query,
    std::vector<std::pair<std::string, double> > &results,
    const int64_t max) {
    RWGuard m(lock_, false);
    stpsearch_.search_by_document(query, results, max);
  }

  /**
   * Search related documents using queries of feature ids.
   * @param query the identifiers of query features
   * @param result search result
   * @param max maximum number of output documents
   */
  void search_by_feature(
    const std::vector<std::string> & query,
    std::vector<std::pair<std::string, double> > &results,
    const int64_t max) {
    RWGuard m(lock_, false);
    stpsearch_.search_by_feature(query, results, max);
  }

  /**
   * Save status.
   * @param filename file name
   * @return return true if sccessed
   */
  bool save(const std::string& filename) {
    std::ofstream ofs(filename.c_str());
    if (!ofs) {
      fprintf(stderr, "Cannot open file %s\n", filename.c_str());
      return false;
    }
    RWGuard m(lock_, false);
    stpsearch_.save(ofs);
    return true;
  }

  /**
   * Load status.
   * @param filename file name
   * @return true if successed
   */
  bool load(const std::string& filename) {
    std::ifstream ifs(filename.c_str());
    if (!ifs) {
      fprintf(stderr, "Cannot open file %s\n", filename.c_str());
      return false;
    }
    RWGuard m(lock_, true);
    stpsearch_.load(ifs);
    return true;
  }
};


}}  /* namespace stupa::evhttp */

#endif // STUPA_STUPA_EVHTTP_H_
