//
// Search server implementation with Thrift
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

#ifndef Search_Handler_H
#define Search_Handler_H

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <concurrency/Mutex.h>
#include "Search.h"
#include "stupa.h"

using namespace apache::thrift;
using namespace apache::thrift::concurrency;

namespace stupa { namespace thrift { /* namespace stupa::thrift */

const int PORT         = 9090;
const int WORKER_COUNT = 4;
const size_t INV_SIZE  = 100;


class SearchHandler : virtual public SearchIf {
 private:
  StupaSearch stpsearch_;  ///< stupa search
  ReadWriteMutex lock_;          ///< read-write lock

 public:
  SearchHandler(size_t invsize, size_t max_doc)
    : stpsearch_(SearchModel::INNER_PRODUCT, invsize, max_doc) { }

  /**
   * Add a document.
   * @param document_id the identifier of input document
   * @param features feature strings of input document
   */
  void add_document(const std::string &document_id,
                    const std::vector<std::string> &features) {
    if (document_id.empty() || features.empty()) return;
    RWGuard m(lock_, 1);
    stpsearch_.add_document(document_id, features);
  }

  /**
   * Delete a document.
   * @param document_id the identifier of document to be deleted
   */
  void delete_document(const std::string &document_id) {
    if (document_id.empty()) return;
    RWGuard m(lock_, 1);
    stpsearch_.delete_document(document_id);
  }

  /**
   * Get the number of documents.
   * @return the number of documents
   */
  int64_t size() {
    RWGuard m(lock_, 0);
    return static_cast<uint64_t>(stpsearch_.size());
  }

  /**
   * Clear status.
   */
  void clear() {
    RWGuard m(lock_, 1);
    stpsearch_.clear();
  }

  /**
   * Search related documents using queries of document ids.
   * @param _return search result
   * @param max maximum number of output documents
   * @param query the identifiers of query documents
   */
  void search_by_document(std::vector<SearchResult> & _return,
                          const int64_t max,
                          const std::vector<std::string> & query) {
    RWGuard m(lock_, 0);
    std::vector<std::pair<std::string, double> > results;
    stpsearch_.search_by_document(query, results, max);
    _return.resize(results.size());
    for (size_t i = 0; i < results.size(); i++) {
      SearchResult sr;
      sr.name = results[i].first;
      sr.point = results[i].second;
      _return[i] = sr;
    }
  }

  /**
   * Search related documents using queries of feature ids.
   * @param _return search result
   * @param max maximum number of output documents
   * @param query the identifiers of query features
   */
  void search_by_feature(std::vector<SearchResult> & _return,
                          const int64_t max,
                          const std::vector<std::string> & query) {
    RWGuard m(lock_, 0);
    std::vector<std::pair<std::string, double> > results;
    stpsearch_.search_by_feature(query, results, max);
    _return.resize(results.size());
    for (size_t i = 0; i < results.size(); i++) {
      SearchResult sr;
      sr.name = results[i].first;
      sr.point = results[i].second;
      _return[i] = sr;
    }
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
    RWGuard m(lock_, 0);
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
    RWGuard m(lock_, 1);
    stpsearch_.load(ifs);
    return true;
  }
};

/**
 * Parameters to start stupa server
 */
struct ServerParam {
  int    port;         ///< port number.
  size_t max_doc;      ///< maximum number of documents.
  int    workerCount;  ///< the number of worker threads.
  size_t invsize;      ///< maximum size of inverted indexes.
  char   *filename;    ///< path of input file.

  ServerParam() : port(PORT), max_doc(0), workerCount(WORKER_COUNT),
                  invsize(INV_SIZE), filename(NULL) { }
};

void usage(const char *progname);
void parse_options(int argc, char **argv, ServerParam &param);

}}  /* namespace stupa::thrift */

#endif  // Search_Handler_H
