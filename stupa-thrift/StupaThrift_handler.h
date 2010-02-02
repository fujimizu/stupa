//
// Search server implementation using Bayesian Sets
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

#ifndef StupaThrift_Handler_H
#define StupaThrift_Handler_H

#include <fstream>
#include <iostream>
#include "StupaThrift.h"
#include <concurrency/Mutex.h>
#include "stupa.h"

using namespace apache::thrift;
using namespace apache::thrift::concurrency;

const int PORT         = 9090;
const int WORKER_COUNT = 4;
const size_t INV_SIZE  = 100;


class StupaThriftHandler : virtual public StupaThriftIf {
 private:
  stupa::BayesianSetsSearch bssearch_;  ///< Bayesian Sets search
  ReadWriteMutex lock_;                 ///< read-write lock

 public:
  StupaThriftHandler(size_t invsize, size_t max_doc)
    : bssearch_(invsize, max_doc) { }

  /**
   * Add a document.
   * @param document_id the identifier of input document
   * @param features feature strings of input document
   */
  void add_document(const std::string &document_id,
                    const std::vector<std::string> &features) {
    if (document_id.empty() || features.empty()) return;
    RWGuard m(lock_, 1);
    bssearch_.add_document(document_id, features);
  }

  /**
   * Delete a document.
   * @param document_id the identifier of document to be deleted
   */
  void delete_document(const std::string &document_id) {
    if (document_id.empty()) return;
    RWGuard m(lock_, 1);
    bssearch_.delete_document(document_id);
  }

  /**
   * Get the number of documents
   * @return the number of documents
   */
  int64_t size() {
    RWGuard m(lock_, 0);
    return static_cast<uint64_t>(bssearch_.size());
  }

  /**
   * Search related documents.
   * @param _return search result
   * @param max maximum number of output documents
   * @param query the identifiers of query documents
   */
  void search(std::vector<SearchResult> &_return, const int64_t max,
              const std::vector<std::string> &query) {
    RWGuard m(lock_, 0);
    std::vector<std::pair<std::string, double> > results;
    bssearch_.search(query, results, max);
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
    bssearch_.save(ofs);
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
    bssearch_.load(ifs);
    return true;
  }
};

struct ServerParam {
  int port;
  size_t max_doc;
  int workerCount;
  size_t invsize;
  char *filename;

  ServerParam() : port(PORT), max_doc(0), workerCount(WORKER_COUNT),
                  invsize(INV_SIZE), filename(NULL) { }
};

void usage(const char *progname);
void parse_options(int argc, char **argv, ServerParam &param);

#endif
