//
// Bayesian Sets class library
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

#ifndef STUPA_BAYESIAN_SETS_H_
#define STUPA_BAYESIAN_SETS_H_

#include <stdint.h>
#include <fstream>
#include <utility>
#include <vector>
#include "config.h"
#include "util.h"

namespace stupa {

/** Type definitions */
typedef uint64_t DocumentId;  ///< type definition of document id
typedef uint64_t FeatureId;   ///< type definition of feature id
typedef double   Point;       ///< type definition of point of vectors
typedef char *   Feature;     ///< type definition of compressed features

/** Constants */
const FeatureId  FEATURE_EMPTY_ID   = 0;  ///< empty_key of feature id
const FeatureId  FEATURE_DELETED_ID = 1;  ///< deleted_key of feature id
const FeatureId  FEATURE_START_ID   = 2;  ///< start number of feature id
const DocumentId DOC_EMPTY_ID       = 0;  ///< empty_key of document id
const DocumentId DOC_DELETED_ID     = 1;  ///< deleted_key of document id
const DocumentId DOC_START_ID       = 2;  ///< start number of document id

/**
 * Bayesian Sets class
 */
class BayesianSets {
 public:
  /** type definition of <document id, compressed feature> map */
  typedef HashMap<DocumentId, Feature>::type DocumentMap;
  /** type definition of <feature id, count of feature id> map */
  typedef HashMap<FeatureId, int>::type FeatureCount;
  /** type definition of the vector of a document */
  typedef HashMap<FeatureId, Point>::type Vector;

 private:
  /** Default value of c parameter of bayesian sets algorithm */
  static const Point  DEFAULT_C = 2.0;
  /** Default value of the maximum number of output documents */
  static const size_t MAX_SEARCH_RESULT = 100;

  DocumentMap documents_;       ///< Documents
  FeatureCount feature_count_;  ///< Count of the features of input documents
  Point c_;                     ///< c parameter of bayesian sets algorithm

  /**
   * Update count of feature ids.
   * @param features list of features to be updated
   * @param flag 1(add) or -1(delete)
   */
  void update_feature_count(const std::vector<FeatureId> &features, int flag);

  /**
   * Make a vector from input query.
   * @param queries input queries(list of document ids)
   * @param query_vector output vector
   */
  void make_query_vector(const std::vector<DocumentId> &queries,
                         Vector &query_vector) const;

  /**
   * Make a weight vector used by search phase.
   * @param query_vector the vector created from input queries
   * @param num_query the number of queries
   * @param weight_vector output vector
   */
  void make_weight_vector(const Vector &query_vector, size_t num_query,
                          Vector &weight_vector) const;

  /**
   * Search related documents.
   * @param query_vector the vector created from input queries
   * @param weight_vector the vector used by weighting output scores
   * @param candidates the candidates of output documents
   * @param results output documents
   * @param max the maximum number of output documents
   */
  void search(const Vector &query_vector, const Vector &weight_vector,
              const std::vector<DocumentId> &candidates,
              std::vector<std::pair<DocumentId, Point> > &results,
              size_t max) const;

 public:
  /**
   * Constructor.
   * @param c 'c' parameter of bayesian sets algorithm
   */
  explicit BayesianSets(Point c = DEFAULT_C) : c_(c) {
    init_hash_map(DOC_EMPTY_ID, documents_);
    init_hash_map(FEATURE_EMPTY_ID, feature_count_);
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
    documents_.set_deleted_key(DOC_DELETED_ID);
    feature_count_.set_deleted_key(FEATURE_DELETED_ID);
#endif
  }

  /**
   * Destructor.
   */
  ~BayesianSets() { clear(); }

  /**
   * Clear documents.
   */
  void clear() {
    for (DocumentMap::iterator it = documents_.begin();
         it != documents_.end(); ++it) {
      if (it->second) delete [] it->second;
    }
    documents_.clear();
    feature_count_.clear();
  }

  /**
   * Get size of documents.
   * @return the size of documents
   */
  size_t size() const { return documents_.size(); }

  /**
   * Get the reference of documents.
   * @return the reference of documents
   */
  const DocumentMap &documents() const { return documents_; }

  /**
   * Get counts of feature ids.
   * @return the reference of count of feature ids
   */
  const FeatureCount &feature_count() const { return feature_count_; }

  /**
   * Get the feature ids of target document.
   * @param id the identifier of target document
   * @param feature_ids output feature ids
   */
  void feature(DocumentId id, std::vector<FeatureId> &feature_ids) const {
    DocumentMap::const_iterator it = documents_.find(id);
    if (it == documents_.end()) return;
    decompress_diff(it->second, feature_ids);
  }

  /**
   * Add a document.
   * @param id the identifier of input document
   * @param feature feature ids of input document
   */
  void add_document(DocumentId id, const std::vector<FeatureId> &feature);

  /**
   * Delete a document.
   * @param id the identifier of target document
   */
  void delete_document(DocumentId id);

  /**
   * Search related documents from all documents using queries of document ids.
   * @param queries the list of document ids
   * @param results output document ids
   * @param max maximum number of output document ids
   */
  void search_by_document(const std::vector<DocumentId> &queries,
                          std::vector<std::pair<DocumentId, Point> > &results,
                          size_t max = MAX_SEARCH_RESULT) const;

  /**
   * Search related documents from candidates using queries of document ids.
   * @param queries the list of document ids
   * @param candidates the candidates of output documents
   * @param results output document ids
   * @param max maximum number of output document ids
   */
  void search_by_document(const std::vector<DocumentId> &queries,
                          const std::vector<DocumentId> &candidates,
                          std::vector<std::pair<DocumentId, Point> > &results,
                          size_t max = MAX_SEARCH_RESULT) const;

  /**
   * Search related documents from all documents using queries of feature ids.
   * @param feature_ids the list of feature ids
   * @param results output document ids
   * @param max maximum number of output document ids
   */
  void search_by_feature(const std::vector<FeatureId> &feature_ids,
                         std::vector<std::pair<DocumentId, Point> > &results,
                         size_t max = MAX_SEARCH_RESULT) const;

  /**
   * Search related documents from candidates using queries of feature ids.
   * @param feature_ids the list of feature ids
   * @param candidates the candidates of output documents
   * @param results output document ids
   * @param max maximum number of output document ids
   */
  void search_by_feature(const std::vector<FeatureId> &feature_ids,
                         const std::vector<DocumentId> &candidates,
                         std::vector<std::pair<DocumentId, Point> > &results,
                         size_t max = MAX_SEARCH_RESULT) const;

  /**
   * Save documents to a file.
   * @param ofs output stream object
   */
  void save(std::ofstream &ofs) const;

  /**
   * Load documents from a file.
   * @param ifs input stream object
   */
  void load(std::ifstream &ifs);
};

} /* namespace stupa */

#endif  // STUPA_BAYESIAN_SETS_H_
