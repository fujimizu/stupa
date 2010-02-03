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

#ifndef STUPA_INVERTED_INDEX_H_
#define STUPA_INVERTED_INDEX_H_

#include <fstream>
#include <vector>
#include "config.h"
#include "bayesian_sets.h"
#include "posting_list.h"
#include "util.h"

namespace stupa {

/**
 * Inverted Index class.
 */
class InvertedIndex {
 public:
  /** Type definition of posting list */
  typedef VarBytePostingList PostingList;
  // typedef VectorPostingList PostingList;

  /** Type definition of <feature id, posting list object> map */
  typedef HashMap<FeatureId, PostingList *>::type IndexHash;

  /** Default value of maximum number of posting lists to be looked up */
  static const size_t MAX_LOOKUP = 1000;

 private:
  IndexHash index_;     ///< posting lists
  size_t max_posting_;  ///< maximum size of posting list

 public:
  /**
   * Constructor.
   * @param max_posting maximum size of posting list
   */
  explicit InvertedIndex(size_t max_posting = 0) : max_posting_(max_posting) {
    init_hash_map(FEATURE_EMPTY_ID, index_);
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
    index_.set_deleted_key(FEATURE_DELETED_ID);
#endif
  }

  /**
   * Destructor.
   */
  ~InvertedIndex() { clear(); }

  /**
   * Get posting lists.
   * @return the reference of posting lists
   */
  const IndexHash &index() const { return index_; }

  /**
   * Get the number of feature ids.
   * @return the number of feature ids
   */
  size_t size() const { return index_.size(); }

  /**
   * Set the maximum size of each posting list.
   * @param max maximum size of posting list
   */
  void set_max(size_t max) { max_posting_ = max; }

  /**
   * Add a documnent into inverted index.
   * @param id the identifier of a document
   * @param feature_ids the feature ids of a document
   */
  void add_document(DocumentId id, const std::vector<FeatureId> &feature_ids);

  /**
   * Delete a document from inverted index.
   * @param id the identifier of a document
   * @param feature_ids the feature ids of a document
   */
  void delete_document(DocumentId id,
                       const std::vector<FeatureId> &feature_ids);

  /**
   * Clear all indexes.
   */
  void clear() {
    for (IndexHash::iterator it = index_.begin(); it != index_.end(); ++it) {
      if (it->second) delete it->second;
    }
    index_.clear();
  }

  /**
   * Look up inverted indexes.
   * @param feature_ids feature ids to be looked up
   * @param documents output list of document ids
   * @param max maximum number of posting lists to be looked up
   */
  void lookup(const std::vector<FeatureId> &feature_ids,
              std::vector<DocumentId> &documents,
              size_t max = MAX_LOOKUP) const;

  /**
   * Save inverted indexes to a file.
   * @param ofs output stream
   */
  void save(std::ofstream &ofs) const;

  /**
   * Load inverted indexes from a file
   * @param ifs input stream
   */
  void load(std::ifstream &ifs);
};

} /* namespace stupa */

#endif  // STUPA_INVERTED_INDEX_H_
