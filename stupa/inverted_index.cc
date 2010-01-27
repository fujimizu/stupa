//
// Inverted index class
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

#include "inverted_index.h"

namespace stupa {

/**
 * Add a documnent into inverted index.
 */
void InvertedIndex::add_document(DocumentId id,
                                 const std::vector<FeatureId> &feature_ids) {
  IndexHash::iterator it;
  for (size_t i = 0; i < feature_ids.size(); i++) {
    it = index_.find(feature_ids[i]);
    if (it != index_.end() && it->second) {
      if (max_posting_ > 0) {
        it->second->add(id, max_posting_);
      } else {
        it->second->add(id);
      }
    } else {
      PostingList *plist = new PostingList;
      plist->add(id);
      index_[feature_ids[i]] = plist;
    }
  }
}

/**
 * Delete document from inverted index.
 */
void InvertedIndex::delete_document(DocumentId id,
                                    const std::vector<FeatureId> &feature_ids) {
  IndexHash::iterator vit;
  for (size_t i = 0; i < feature_ids.size(); i++) {
    vit = index_.find(feature_ids[i]);
    if (vit != index_.end() && vit->second) {
      vit->second->remove(id);
      if (vit->second->empty()) {
        delete vit->second;
        index_.erase(feature_ids[i]);
      }
    }
  }
}

/**
 * Look up inverted indexes.
 */
void InvertedIndex::lookup(const std::vector<FeatureId> &feature_ids,
                           std::vector<DocumentId> &documents,
                           size_t max) const {
  HashMap<DocumentId, size_t>::type count;
  HashMap<DocumentId, size_t>::type::iterator cit;
  init_hash_map(DOC_EMPTY_ID, count);
  std::vector<DocumentId> document_ids;
  for (size_t i = 0; i < feature_ids.size(); i++) {
    IndexHash::const_iterator it = index_.find(feature_ids[i]);
    if (it != index_.end() && it->second) {
      it->second->list(document_ids);
      for (size_t j = 0; j < document_ids.size(); j++) {
        cit = count.find(document_ids[j]);
        if (cit == count.end()) {
          count[document_ids[j]] = 1;
        } else {
          cit->second++;
        }
      }
      document_ids.clear();
    }
  }

  if (count.size() > max) {
    size_t cnt = 0;
    std::vector<std::pair<DocumentId, size_t> > pairs(count.size());
    for (cit = count.begin(); cit != count.end(); ++cit) {
      pairs[cnt++] = std::pair<DocumentId, size_t>(cit->first, cit->second);
    }
    std::sort(pairs.begin(), pairs.end(), greater_pair<DocumentId, size_t>);
    for (size_t i = 0; i < pairs.size() && i < max; i++) {
      documents.push_back(pairs[i].first);
    }
  } else {
    for (cit = count.begin(); cit != count.end(); ++cit) {
      documents.push_back(cit->first);
    }
  }
}

/**
 * Save inverted indexes to a file.
 */
void InvertedIndex::save(std::ofstream &ofs) const {
  ofs.write((const char *)&max_posting_, sizeof(max_posting_));
  size_t isiz = index_.size();
  ofs.write((const char *)&isiz, sizeof(isiz));
  for (IndexHash::const_iterator it = index_.begin();
       it != index_.end(); ++it) {
    ofs.write((const char *)&it->first, sizeof(it->first));
    it->second->save(ofs);
  }
}

/**
 * Load inverted indexes from a file
 */
void InvertedIndex::load(std::ifstream &ifs) {
  clear();
  ifs.read((char *)&max_posting_, sizeof(max_posting_));
  size_t isiz;
  ifs.read((char *)&isiz, sizeof(isiz));
  for (size_t i = 0; i < isiz; i++) {
    FeatureId fid;
    ifs.read((char *)&fid, sizeof(fid));
    PostingList *plist = new PostingList;
    plist->load(ifs);
    index_[fid] = plist;
  }
}

} /* namespace stupa */
