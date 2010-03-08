//
// Stupa Search using inverted index
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

#include <algorithm>
#include <set>
#include "search.h"

namespace stupa {

/**
 * Look up inverted indexes.
 */
void StupaSearch::lookup_inverted_index_by_document(
  const std::vector<DocumentId> &queries,
  std::vector<DocumentId> &results) const {
  std::set<FeatureId> fidset;
  std::vector<FeatureId> feature_ids;
  for (size_t i = 0; i < queries.size(); i++) {
    model_->feature(queries[i], feature_ids);
    for (size_t i = 0; i < feature_ids.size(); i++) {
      fidset.insert(feature_ids[i]);
    }
    feature_ids.clear();
  }
  for (std::set<FeatureId>::iterator it = fidset.begin();
       it != fidset.end(); ++it) {
    feature_ids.push_back(*it);
  }
  inv_.lookup(feature_ids, results);
}

/**
 * Delete the oldest document.
 */
void StupaSearch::delete_oldest_document() {
  std::vector<FeatureId> features;
  model_->feature(oldest_document_id_, features);
  inv_.delete_document(oldest_document_id_, features);
  model_->delete_document(oldest_document_id_);
  str2did_.erase(did2str_[oldest_document_id_]);
  did2str_.erase(oldest_document_id_);
  oldest_document_id_++;
  while (oldest_document_id_ <= current_document_id_
         && did2str_.find(oldest_document_id_) == did2str_.end()) {
    oldest_document_id_++;
  }
}

/**
 * Add a document to search model object and inverted indexes.
 */
void StupaSearch::add_document(const std::string &document_id,
                                const std::vector<std::string> &features) {
  if (document_id.empty() || features.empty()) return;
  while (max_documents_ && model_->size() >= max_documents_) {
    delete_oldest_document();
  }

  std::vector<FeatureId> feature_ids;
  Str2FeatureId::iterator fit;
  for (size_t i = 0; i < features.size(); i++) {
    if (features[i].empty()) continue;
    fit = str2fid_.find(features[i]);
    FeatureId feature_id;
    if (fit == str2fid_.end()) {
      feature_id = current_feature_id_++;
      str2fid_[features[i]] = feature_id;
    } else {
      feature_id = fit->second;
    }
    feature_ids.push_back(feature_id);
  }
  std::sort(feature_ids.begin(), feature_ids.end());

  Str2DocId::iterator dit = str2did_.find(document_id);
  if (dit != str2did_.end()) {
    std::vector<FeatureId> old_feature;
    model_->feature(dit->second, old_feature);
    inv_.delete_document(dit->second, old_feature);
    model_->add_document(dit->second, feature_ids);
    inv_.add_document(dit->second, feature_ids);
  } else {
    str2did_[document_id] = current_document_id_;
    did2str_[current_document_id_] = document_id;
    model_->add_document(current_document_id_, feature_ids);
    inv_.add_document(current_document_id_, feature_ids);
    current_document_id_++;
  }
}

/**
 * Delete a document from search model object and inverted indexes.
 * @param document_id identifier string of a document
 */
void StupaSearch::delete_document(const std::string &document_id) {
  Str2DocId::iterator sdit = str2did_.find(document_id);
  if (sdit != str2did_.end()) {
    if (sdit->second == oldest_document_id_) {
      delete_oldest_document();
    } else {
      std::vector<FeatureId> features;
      model_->feature(sdit->second, features);
      inv_.delete_document(sdit->second, features);
      model_->delete_document(sdit->second);
      DocId2Str::iterator dsit = did2str_.find(sdit->second);
      if (dsit != did2str_.end()) did2str_.erase(dsit);
      str2did_.erase(sdit);
    }
  }
}

/**
 * Search related documents using queries of document ids.
 */
void StupaSearch::search_by_document(
  const std::vector<std::string> &queries,
  std::vector<std::pair<std::string, Point> > &results, size_t max) const {
  std::vector<DocumentId> document_ids;
  for (size_t i = 0; i < queries.size(); i++) {
    Str2DocId::const_iterator it = str2did_.find(queries[i]);
    if (it != str2did_.end()) document_ids.push_back(it->second);
  }
  if (document_ids.empty()) return;

  std::vector<DocumentId> candidates;
  lookup_inverted_index_by_document(document_ids, candidates);
  std::vector<std::pair<DocumentId, Point> > pairs;
  model_->search_by_document(document_ids, candidates, pairs, max);
  for (size_t i = 0; i < pairs.size(); i++) {
    DocId2Str::const_iterator it = did2str_.find(pairs[i].first);
    if (it != did2str_.end()) {
      results.push_back(
        std::pair<std::string, Point>(it->second, pairs[i].second));
    }
  }
}

/**
 * Search related documents using queries of feature ids.
 */
void StupaSearch::search_by_feature(
  const std::vector<std::string> &queries,
  std::vector<std::pair<std::string, Point> > &results, size_t max) const {
  std::set<FeatureId> fidset;
  for (size_t i = 0; i < queries.size(); i++) {
    Str2FeatureId::const_iterator it = str2fid_.find(queries[i]);
    if (it != str2fid_.end()) fidset.insert(it->second);
  }
  std::vector<FeatureId> feature_ids;
  for (std::set<FeatureId>::iterator it = fidset.begin();
       it != fidset.end(); ++it) {
    feature_ids.push_back(*it);
  }
  if (feature_ids.empty()) return;

  std::vector<DocumentId> candidates;
  inv_.lookup(feature_ids, candidates);
  std::vector<std::pair<DocumentId, Point> > pairs;
  model_->search_by_feature(feature_ids, candidates, pairs, max);
  for (size_t i = 0; i < pairs.size(); i++) {
    DocId2Str::const_iterator it = did2str_.find(pairs[i].first);
    if (it != did2str_.end()) {
      results.push_back(
        std::pair<std::string, Point>(it->second, pairs[i].second));
    }
  }
}

/**
 * Save status (search model object, inverted indexes, ..) to a file.
 */
void StupaSearch::save(std::ofstream &ofs) const {
  ofs.write((const char *)&current_feature_id_, sizeof(current_feature_id_));
  ofs.write((const char *)&current_document_id_, sizeof(current_document_id_));
  ofs.write((const char *)&oldest_document_id_, sizeof(oldest_document_id_));
  model_->save(ofs);
  inv_.save(ofs);

  size_t size;
  size = did2str_.size();
  ofs.write((const char *)&size, sizeof(size));
  for (DocId2Str::const_iterator it = did2str_.begin();
       it != did2str_.end(); ++it) {
    ofs.write((const char *)&it->first, sizeof(it->first));
    size_t ssize = it->second.size();
    ofs.write((const char *)&ssize, sizeof(ssize));
    ofs.write((const char *)&it->second[0], sizeof(it->second[0]) * ssize);
  }

  size = str2fid_.size();
  ofs.write((const char *)&size, sizeof(size));
  for (Str2FeatureId::const_iterator it = str2fid_.begin();
       it != str2fid_.end(); ++it) {
    size_t ssize = it->first.size();
    ofs.write((const char *)&ssize, sizeof(ssize));
    ofs.write((const char *)&it->first[0], sizeof(it->first[0]) * ssize);
    ofs.write((const char *)&it->second, sizeof(it->second));
  }

  size = str2did_.size();
  ofs.write((const char *)&size, sizeof(size));
  for (Str2DocId::const_iterator it = str2did_.begin();
       it != str2did_.end(); ++it) {
    size_t ssize = it->first.size();
    ofs.write((const char *)&ssize, sizeof(ssize));
    ofs.write((const char *)&it->first[0], sizeof(it->first[0]) * ssize);
    ofs.write((const char *)&it->second, sizeof(it->second));
  }
}

/**
 * Load status (search model object, inverted indexes, ..) from a file.
 */
void StupaSearch::load(std::ifstream &ifs) {
  clear();
  ifs.read((char *)&current_feature_id_, sizeof(current_feature_id_));
  ifs.read((char *)&current_document_id_, sizeof(current_document_id_));
  ifs.read((char *)&oldest_document_id_, sizeof(oldest_document_id_));
  model_->load(ifs);
  inv_.load(ifs);

  size_t size;
  ifs.read((char *)&size, sizeof(size));
  for (size_t i = 0; i < size; i++) {
    DocumentId did;
    ifs.read((char *)&did, sizeof(did));
    std::string str;
    size_t ssize;
    ifs.read((char *)&ssize, sizeof(ssize));
    str.resize(ssize);
    ifs.read((char *)&str[0], sizeof(str[0]) * ssize);
    did2str_[did] = str;
  }

  ifs.read((char *)&size, sizeof(size));
  for (size_t i = 0; i < size; i++) {
    FeatureId fid;
    std::string str;
    size_t ssize;
    ifs.read((char *)&ssize, sizeof(ssize));
    str.resize(ssize);
    ifs.read((char *)&str[0], sizeof(str[0]) * ssize);
    ifs.read((char *)&fid, sizeof(fid));
    str2fid_[str] = fid;
  }

  ifs.read((char *)&size, sizeof(size));
  for (size_t i = 0; i < size; i++) {
    DocumentId did;
    std::string str;
    size_t ssize;
    ifs.read((char *)&ssize, sizeof(ssize));
    str.resize(ssize);
    ifs.read((char *)&str[0], sizeof(str[0]) * ssize);
    ifs.read((char *)&did, sizeof(did));
    str2did_[str] = did;
  }

  while (max_documents_ && model_->size() > max_documents_) {
    delete_oldest_document();
  }
}

/**
 * Read input file and add documents.
 */
void StupaSearch::read_tsvfile(std::ifstream &ifs) {
  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      size_t p = line.find(stupa::DELIMITER);
      std::string doc_name = line.substr(0, p);
      line = line.substr(p + stupa::DELIMITER.size());
      std::vector<std::string> features;
      stupa::split_string(line, stupa::DELIMITER, features);
      if (!doc_name.empty() && !features.empty()) {
        add_document(doc_name, features);
      }
    }
  }
}

} /* namespace stupa */
