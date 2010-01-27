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

#include "Stupa.h"
#include <concurrency/Mutex.h>
#include "stupa.h"

using namespace apache::thrift;
using namespace apache::thrift::concurrency;

class StupaHandler : virtual public StupaIf {
 private:
  stupa::BayesianSetsSearch bssearch_;  ///< Bayesian Sets search
  ReadWriteMutex lock_;                 ///< read-write lock

 public:
  StupaHandler(size_t invsize) : bssearch_(invsize) { }

  void add_document(const std::string &document_id,
                    const std::vector<std::string> &features) {
    if (document_id.empty() || features.empty()) return;
    RWGuard m(lock_, 1);
    bssearch_.add_document(document_id, features);
  }

  void delete_document(const std::string &document_id) {
    if (document_id.empty()) return;
    RWGuard m(lock_, 1);
    bssearch_.delete_document(document_id);
  }

  int64_t size() {
    RWGuard m(lock_, 0);
    return static_cast<uint64_t>(bssearch_.size());
  }

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
};
