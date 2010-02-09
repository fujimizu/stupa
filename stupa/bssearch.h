//
// Bayesian Sets Search class using bayesian sets algorithm and inverted index
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

#ifndef STUPA_BSSEARCH_H_
#define STUPA_BSSEARCH_H_

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include "bayesian_sets.h"
#include "inverted_index.h"
#include "util.h"

namespace stupa {

/**
 * Bayesian Sets Search class
 * (Bayesian Sets with Inverted Index)
 */
class BayesianSetsSearch {
 private:
  /** Type definition of <document id, string> map */
  typedef HashMap<DocumentId, std::string>::type DocId2Str;
  /** Type definition of <string, document id> map */
  typedef HashMap<std::string, DocumentId>::type Str2DocId;
  /** Type definition of <string, feature id> map */
  typedef HashMap<std::string, FeatureId>::type Str2FeatureId;

  /** maximum number of search results */
  static const size_t MAX_RESULT      = 20;
  /** maximum size of inverted index */
  static const size_t MAX_INVERT_SIZE = 100;

  BayesianSets bs_;                 ///< bayesian sets object
  InvertedIndex inv_;               ///< inverted index
  FeatureId current_feature_id_;    ///< current(highest) feature id
  DocumentId current_document_id_;  ///< current(highest) document id
  DocumentId oldest_document_id_;   ///< oldest document id
  DocId2Str did2str_;               ///< mapping from document id to string
  Str2DocId str2did_;               ///< mapping from string to document id
  Str2FeatureId str2fid_;           ///< mapping from string to feature id
  size_t max_documents_;            ///< maximum number of documents

  /**
   * Look up inverted index.
   * @param queries list of document ids of input queries
   * @paran results list of document ids of output candidates
   */
  void lookup_inverted_index(const std::vector<DocumentId> &queries,
                             std::vector<DocumentId> &results) const;

  /**
   * Delete the oldest document.
   */
  void delete_oldest_document();

 public:
  /**
   * Constructor.
   * @param invsize maximum size of inverted indexes
   * @param max_doc maximum number of documents
   */
  BayesianSetsSearch(size_t invsize = MAX_INVERT_SIZE,
                     size_t max_doc = 0,
                     Point c = BayesianSets::DEFAULT_C)
    : bs_(c), inv_(invsize),
      current_feature_id_(FEATURE_START_ID),
      current_document_id_(DOC_START_ID),
      oldest_document_id_(DOC_START_ID),
      max_documents_(max_doc) {
    init_hash_map(DOC_EMPTY_ID, did2str_);
    init_hash_map("", str2did_);
    init_hash_map("", str2fid_);
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
    did2str_.set_deleted_key(DOC_DELETED_ID);
    str2did_.set_deleted_key(DELIMITER);
    str2fid_.set_deleted_key(DELIMITER);
#endif
  }

  /**
   * Destructor.
   */
  ~BayesianSetsSearch() { }

  /**
   * Get the number of stored documents.
   * @return the number of stored documents
   */
  size_t size() const { return bs_.size(); }

  /**
   * Add a document to bayesian sets object and inverted indexes.
   * @param document_id identifier string of a document
   * @param features feature strings of a document
   */
  void add_document(const std::string& document_id,
                    const std::vector<std::string> &features);

  /**
   * Delete a document from bayesian sets object and inverted indexes.
   * @param document_id identifier string of a document
   */
  void delete_document(const std::string& document_id);

  /**
   * Clear documents from bayesian sets object and inverted index, mapping,
   * and initialize identifiers of documents and features.
   */
  void clear() {
    bs_.clear();
    inv_.clear();
    did2str_.clear();
    str2did_.clear();
    str2fid_.clear();
    current_feature_id_ = FEATURE_START_ID;
    current_document_id_ = DOC_START_ID;
    oldest_document_id_ = DOC_START_ID;
  }

  /**
   * Search related documents.
   * @param queries list of query strings as document identifiers
   * @param results list of the pairs of document-identifier string and points
   * @param max maximum number of output pairs
   */
  void search(const std::vector<std::string> &queries,
              std::vector<std::pair<std::string, Point> > &results,
              size_t max = MAX_RESULT) const;

  /**
   * Save status (bayesian sets object, inverted indexes, ..) to a file.
   * @param ofs output stream
   */
  void save(std::ofstream &ofs) const;

  /**
   * Load status (bayesian sets object, inverted indexes, ..) from a file.
   * @param ifs input stream
   */
  void load(std::ifstream &ifs);

  /**
   * Read input file and add documents to BayesianSets searcher.
   * @param ifs input stream
   */
  void read_tsvfile(std::ifstream &ifs);
};

} /* namespace stupa */

#endif  // STUPA_BSSEARCH_H_
