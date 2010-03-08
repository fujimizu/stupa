//
// Search Model class
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

#ifndef STUPA_SEARCH_MODEL_H_
#define STUPA_SEARCH_MODEL_H_

#include <stdint.h>
#include <fstream>
#include <utility>
#include <vector>
#include "config.h"
#include "identifier.h"
#include "util.h"

namespace stupa {

/**
 * Search Model class (virtual class)
 */
class SearchModel {
 public:
  /** type definition of <document id, compressed feature> map */
  typedef HashMap<DocumentId, Feature>::type DocumentMap;
  /** type definition of <feature id, count of feature id> map */
  typedef HashMap<FeatureId, int>::type FeatureCount;
  /** type definition of the vector of a document */
  typedef HashMap<FeatureId, Point>::type Vector;

  enum Type {
    INNER_PRODUCT,
    COSINE,
  };

 protected:
  DocumentMap documents_;       ///< Documents
  FeatureCount feature_count_;  ///< Count of the features of input documents

  /**
   * Apply IDF(inverse document frequency) weighting.
   * @param vec input vector
   */
  void idf(Vector &vec) const {
    size_t ndocs = documents_.size();
    FeatureCount::const_iterator fit;
    for (Vector::iterator vit = vec.begin(); vit != vec.end(); ++vit) {
      fit = feature_count_.find(vit->first);
      if (fit != feature_count_.end() && fit->second > 0) {
        Point val = log(ndocs / fit->second) + 1;
        vit->second *= val;
      }
    }
  }

  /**
   * Get norm value.
   * @param vec input vector
   * @return norm value
   */
  Point norm(const Vector &vec) const {
    Point result = 0.0;
    for (Vector::const_iterator vit = vec.begin(); vit != vec.end(); ++vit) {
        result += vit->second * vit->second;
    }
    return sqrt(result);
  }

  /**
   * Normalize a vector.
   * @param vec input vector
   */
  void normalize(Vector &vec) const {
    Point nrm = norm(vec);
    if (nrm > 0) {
      for (Vector::iterator vit = vec.begin(); vit != vec.end(); ++vit) {
        vit->second /= nrm;
      }
    }
  }

  /**
   * Calculate inner product value between input vectors.
   * @param vec1 input vector
   * @param vec2 input vector
   * @return inner product value
   */
  Point inner_product(const Vector &vec1, const Vector &vec2) const {
    Vector::const_iterator it, itother, end;
    const Vector *other;
    if (vec1.size() < vec2.size()) {
      it = vec1.begin();
      end = vec1.end();
      other = &vec2;
    } else {
      it = vec2.begin();
      end = vec2.end();
      other = &vec1;
    }
    Point prod = 0;
    while (it != end) {
      itother = other->find(it->first);
      if (itother != other->end()) {
        prod += it->second * itother->second;
      }
      ++it;
    }
    return prod;
  }

 private:
  /**
   * Search related documents.
   * @param query_vector the vector created from input queries
   * @param candidates the candidates of output documents
   * @param results output documents
   * @param max the maximum number of output documents
   */
  virtual void search(Vector &query_vector,
                      const std::vector<DocumentId> &candidates,
                      std::vector<std::pair<DocumentId, Point> > &results,
                      size_t max) const = 0;

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

 public:
  /**
   * Constructor.
   */
  SearchModel() {
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
  virtual ~SearchModel() = 0;

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
                          size_t max) const;

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
                          size_t max) const;

  /**
   * Search related documents from all documents using queries of feature ids.
   * @param feature_ids the list of feature ids
   * @param results output document ids
   * @param max maximum number of output document ids
   */
  void search_by_feature(const std::vector<FeatureId> &feature_ids,
                         std::vector<std::pair<DocumentId, Point> > &results,
                         size_t max) const;

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
                         size_t max) const;

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

  static void print_vector(const Vector &vec) {
    for (Vector::const_iterator it = vec.begin(); it != vec.end(); ++it) {
      if (it != vec.begin()) printf("\t");
      printf("%ld:%.4f", it->first, it->second);
    }
    printf("\n");
  }
};


/**
 * Search model using IDF weighting and inner product method
 */
class SearchModelInnerProduct : public SearchModel {
 private:
  /**
   * Search related documents.
   * @param query_vector the vector created from input queries
   * @param candidates the candidates of output documents
   * @param results output documents
   * @param max the maximum number of output documents
   */
  void search(Vector &query_vector,
              const std::vector<DocumentId> &candidates,
              std::vector<std::pair<DocumentId, Point> > &results,
              size_t max) const {
    idf(query_vector);
    std::vector<FeatureId> feature_ids;
    std::vector<std::pair<DocumentId, Point> > pairs;
    DocumentMap::const_iterator dit;
    Vector::const_iterator vit;
    for (size_t i = 0; i < candidates.size(); i++) {
      dit = documents_.find(candidates[i]);
      if (dit != documents_.end()) {
        decompress_diff(dit->second, feature_ids);
        Point score = 0.0;
        for (size_t j = 0; j < feature_ids.size(); j++) {
          vit = query_vector.find(feature_ids[j]);
          if (vit != query_vector.end()) score += vit->second * vit->second;
        }
        if (score != 0) {
          pairs.push_back(std::pair<DocumentId, Point>(dit->first, score));
        }
        feature_ids.clear();
      }
    }
    if (pairs.size() <= max) {
      std::sort(pairs.begin(), pairs.end(),
                greater_pair<DocumentId, Point>);
      std::copy(pairs.begin(), pairs.end(), back_inserter(results));
    } else {
      std::partial_sort(pairs.begin(), pairs.begin() + max, pairs.end(),
                        greater_pair<DocumentId, Point>);
      std::copy(pairs.begin(), pairs.begin() + max, back_inserter(results));
    }
  }

 public:
  ~SearchModelInnerProduct() { clear(); }
};

/**
 * Search model using IDF weighting and cosine similarity method
 */
class SearchModelCosine : public SearchModel {
 private:
  /**
   * Search related documents.
   * @param query_vector the vector created from input queries
   * @param candidates the candidates of output documents
   * @param results output documents
   * @param max the maximum number of output documents
   */
  void search(Vector &query_vector,
              const std::vector<DocumentId> &candidates,
              std::vector<std::pair<DocumentId, Point> > &results,
              size_t max) const {
    idf(query_vector);
    normalize(query_vector);

    size_t ndocs = documents_.size();
    std::vector<FeatureId> feature_ids;
    std::vector<std::pair<DocumentId, Point> > pairs;
    DocumentMap::const_iterator dit;
    Vector::iterator vit;
    FeatureCount::const_iterator fit;
    for (size_t i = 0; i < candidates.size(); i++) {
      dit = documents_.find(candidates[i]);
      if (dit == documents_.end()) continue;
      decompress_diff(dit->second, feature_ids);
      Point score = 0.0;
      Point norm = 0.0;
      for (size_t j = 0; j < feature_ids.size(); j++) {
        fit = feature_count_.find(feature_ids[j]);
        if (fit != feature_count_.end() && fit->second > 0) {
          Point val = log(ndocs / fit->second) + 1;
          norm += val * val;
          vit = query_vector.find(feature_ids[j]);
          if (vit != query_vector.end()) {
            score += val * vit->second;
          }
        }
      }
      if (norm != 0 && score != 0) {
        score /= sqrt(norm);
        pairs.push_back(std::pair<DocumentId, Point>(dit->first, score));
      }
      feature_ids.clear();
    }

    if (pairs.size() <= max) {
      std::sort(pairs.begin(), pairs.end(),
                greater_pair<DocumentId, Point>);
      std::copy(pairs.begin(), pairs.end(), back_inserter(results));
    } else {
      std::partial_sort(pairs.begin(), pairs.begin() + max, pairs.end(),
                        greater_pair<DocumentId, Point>);
      std::copy(pairs.begin(), pairs.begin() + max, back_inserter(results));
    }
  }

 public:
  ~SearchModelCosine() { clear(); }
};

} /* namespace stupa */

#endif  // STUPA_SEARCH_MODEL_H_
