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

#include <cmath>
#include <cassert>
#include <algorithm>
#include "search_model.h"

namespace stupa {

/**
 * Deconstructor.
 */
SearchModel::~SearchModel() { }

/**
 * Update count of feature ids.
 */
void SearchModel::update_feature_count(const std::vector<FeatureId> &features,
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
void SearchModel::make_query_vector(const std::vector<DocumentId> &queries,
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
 * Add a document.
 */
void SearchModel::add_document(DocumentId id,
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
void SearchModel::delete_document(DocumentId id) {
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
void SearchModel::search_by_document(
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
void SearchModel::search_by_document(
  const std::vector<DocumentId> &queries,
  const std::vector<DocumentId> &candidates,
  std::vector<std::pair<DocumentId, Point> > &results, size_t max) const {
  Vector query_vector;
  init_hash_map(FEATURE_EMPTY_ID, query_vector);
  make_query_vector(queries, query_vector);
  search(query_vector, candidates, results, max);
}

/**
 * Search related documents using queries of feature ids.
 */
void SearchModel::search_by_feature(
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
void SearchModel::search_by_feature(
  const std::vector<FeatureId> &feature_ids,
  const std::vector<DocumentId> &candidates,
  std::vector<std::pair<DocumentId, Point> > &results, size_t max) const {
  Vector query_vector;
  init_hash_map(FEATURE_EMPTY_ID, query_vector);
  for (size_t i = 0; i < feature_ids.size(); i++) {
    query_vector[feature_ids[i]] = 1.0;
  }
  search(query_vector, candidates, results, max);
}

/**
 * Save documents to a file.
 */
void SearchModel::save(std::ofstream &ofs) const {
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
void SearchModel::load(std::ifstream &ifs) {
  clear();
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
