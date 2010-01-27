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

#include "bayesian_sets.h"

namespace stupa {

/**
 * Update count of feature ids.
 */
void BayesianSets::update_feature_count(const std::vector<FeatureId> &features,
                                        int flag) {
  FeatureCount::iterator it;
  for (size_t i = 0; i < features.size(); i++) {
    int val = flag;
    it = feature_count_.find(features[i]);
    if (it != feature_count_.end()) val += it->second;
    if (val > 0) {
      feature_count_[features[i]] = val;
    } else {
      feature_count_.erase(features[i]);
    }
  }
}

/**
 * Make a vector from input query.
 */
void BayesianSets::make_query_vector(const std::vector<DocumentId> &queries,
                                     Vector &query_vector) const {
  Vector::iterator vit;
  DocumentMap::const_iterator dit;
  for (size_t i = 0; i < queries.size(); i++) {
    dit = documents_.find(queries[i]);
    if (dit == documents_.end()) continue;

    std::vector<FeatureId> feature_ids;
    decompress_diff(dit->second, feature_ids);
    for (size_t j = 0; j < feature_ids.size(); j++) {
      Point val = 1;
      vit = query_vector.find(feature_ids[j]);
      if (vit != query_vector.end()) val += vit->second;
      query_vector[feature_ids[j]] = val;
    }
  }
}

/**
 * Make a weight vector used by search phase.
 */
void BayesianSets::make_weight_vector(const Vector &query_vector,
                                      size_t num_query,
                                      Vector &weight_vector) const {
  size_t siz = documents_.size();
  FeatureCount::const_iterator fit;
  for (Vector::const_iterator qit = query_vector.begin();
       qit != query_vector.end(); ++qit) {
    fit = feature_count_.find(qit->first);
    if (fit != feature_count_.end() && fit->second > 0) {
      Point average = static_cast<Point>(fit->second) / siz;
      Point val_alpha = log(1.0 + qit->second / (c_ * average));
      Point val_beta = (average == 1.0) ?
        0 : log(1.0 + (num_query - qit->second) / (c_ * (1.0 - average)));
      double val_weight = val_alpha - val_beta;
      if (!std::isnan(val_weight) && val_weight != 0) {
        weight_vector[qit->first] = val_weight;
      }
    }
  }
}

/**
 * Search related documents.
 */
void BayesianSets::search(const Vector &query_vector,
                          const Vector &weight_vector,
                          const std::vector<DocumentId> &candidates,
                          std::vector<std::pair<DocumentId, Point> > &results,
                          size_t max) const {
  DocumentMap::const_iterator dit;
  Vector::const_iterator vit;
  std::vector<std::pair<DocumentId, Point> > pairs;
  for (size_t i = 0; i < candidates.size(); i++) {
    dit = documents_.find(candidates[i]);
    if (dit != documents_.end()) {
      std::vector<FeatureId> feature_ids;
      decompress_diff(dit->second, feature_ids);
      Point score = 0.0;
      for (size_t j = 0; j < feature_ids.size(); j++) {
        vit = weight_vector.find(feature_ids[j]);
        if (vit != weight_vector.end()) score += vit->second;
      }
      if (score != 0) {
        pairs.push_back(std::pair<DocumentId, Point>(candidates[i], score));
      }
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

/**
 * Add a document.
 */
void BayesianSets::add_document(DocumentId id,
                                const std::vector<FeatureId> &feature) {
  assert(id != DOC_EMPTY_ID);
  assert(id != DOC_DELETED_ID);

  DocumentMap::iterator it = documents_.find(id);
  if (it != documents_.end()) {
    std::vector<FeatureId> feature_ids;
    decompress_diff(it->second, feature_ids);
    update_feature_count(feature_ids, -1);
    if (it->second) delete [] it->second;
  }
  update_feature_count(feature, 1);
  Feature f = compress_diff(feature);
  documents_[id] = f;
}

/**
 * Delete a document.
 */
void BayesianSets::delete_document(DocumentId id) {
  assert(id != DOC_EMPTY_ID);
  assert(id != DOC_DELETED_ID);

  DocumentMap::iterator it = documents_.find(id);
  if (it != documents_.end()) {
    std::vector<FeatureId> feature_ids;
    decompress_diff(it->second, feature_ids);
    update_feature_count(feature_ids, -1);
    if (it->second) delete [] it->second;
    documents_.erase(id);
  }
}

/**
 * Search related documents using queries of document ids
 */
void BayesianSets::search_by_document(
  const std::vector<DocumentId> &queries,
  std::vector<std::pair<DocumentId, Point> > &results, size_t max) const {
  std::vector<DocumentId> candidates;
  for (DocumentMap::const_iterator it = documents_.begin();
       it != documents_.end(); ++it) {
    candidates.push_back(it->first);
  }
  search_by_document(queries, candidates, results, max);
}

/**
 * Search related documents using queries of document ids
 */
void BayesianSets::search_by_document(
  const std::vector<DocumentId> &queries,
  const std::vector<DocumentId> &candidates,
  std::vector<std::pair<DocumentId, Point> > &results, size_t max) const {
  Vector query_vector, weight_vector;
  init_hash_map(FEATURE_EMPTY_ID, query_vector);
  init_hash_map(FEATURE_EMPTY_ID, weight_vector);
  make_query_vector(queries, query_vector);
  make_weight_vector(query_vector, queries.size(), weight_vector);
  search(query_vector, weight_vector, candidates, results, max);
}

/**
 * Search related documents using queries of feature ids.
 */
void BayesianSets::search_by_feature(
  const std::vector<FeatureId> &feature_ids,
  std::vector<std::pair<DocumentId, Point> > &results, size_t max) const {
  std::vector<DocumentId> candidates;
  for (DocumentMap::const_iterator it = documents_.begin();
       it != documents_.end(); ++it) {
    candidates.push_back(it->first);
  }
  search_by_feature(feature_ids, candidates, results, max);
}

/**
 * Search related documents using queries of feature ids.
 */
void BayesianSets::search_by_feature(
  const std::vector<FeatureId> &feature_ids,
  const std::vector<DocumentId> &candidates,
  std::vector<std::pair<DocumentId, Point> > &results, size_t max) const {
  Vector query_vector, weight_vector;
  init_hash_map(FEATURE_EMPTY_ID, query_vector);
  init_hash_map(FEATURE_EMPTY_ID, weight_vector);
  for (size_t i = 0; i < feature_ids.size(); i++) {
    query_vector[feature_ids[i]] = 1.0;
  }
  make_weight_vector(query_vector, 1, weight_vector);
  search(query_vector, weight_vector, candidates, results, max);
}

/**
 * Save documents to a file.
 */
void BayesianSets::save(std::ofstream &ofs) const {
  ofs.write((const char *)&c_, sizeof(c_));
  size_t dsiz = documents_.size();
  ofs.write((const char *)&dsiz, sizeof(dsiz));
  for (DocumentMap::const_iterator it = documents_.begin();
       it != documents_.end(); ++it) {
    ofs.write((const char *)&it->first, sizeof(it->first));
    size_t fsiz = sizeof_compressed(it->second);
    ofs.write((const char *)&fsiz, sizeof(fsiz));
    ofs.write((const char *)it->second, fsiz);
  }
  size_t fcsiz = feature_count_.size();
  ofs.write((const char *)&fcsiz, sizeof(fcsiz));
  for (FeatureCount::const_iterator it = feature_count_.begin();
       it != feature_count_.end(); ++it) {
    ofs.write((const char *)&it->first, sizeof(it->first));
    ofs.write((const char *)&it->second, sizeof(it->second));
  }
}

/**
 * Load documents from a file.
 */
void BayesianSets::load(std::ifstream &ifs) {
  clear();
  ifs.read((char *)&c_, sizeof(c_));
  size_t dsiz;
  ifs.read((char *)&dsiz, sizeof(dsiz));
  for (size_t i = 0; i < dsiz; i++) {
    DocumentId did;
    ifs.read((char *)&did, sizeof(did));
    size_t fsiz;
    ifs.read((char *)&fsiz, sizeof(fsiz));
    Feature feature = new char[fsiz];
    ifs.read((char *)feature, fsiz);
    documents_[did] = feature;
  }
  size_t fcsiz;
  ifs.read((char *)&fcsiz, sizeof(fcsiz));
  for (size_t i = 0; i < fcsiz; i++) {
    FeatureId fid;
    ifs.read((char *)&fid, sizeof(fid));
    int count;
    ifs.read((char *)&count, sizeof(count));
    feature_count_[fid] = count;
  }
}

} /* namespace stupa */
