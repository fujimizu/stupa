//
// Posting List class
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

#ifndef STUPA_POSTING_LIST_H_
#define STUPA_POSTING_LIST_H_

#include <stdint.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include "util.h"

namespace stupa {

/**
 * Posting list using std::vector container.
 */
class VectorPostingList {
 private:
  std::vector<uint64_t> plist_;  ///< list of document ids

 public:
  /** Constructor */
  VectorPostingList() { }

  /** Destructor */
  ~VectorPostingList() { }

  /**
   * Add the identifier of a document id.
   * @param id the identifier of a document
   */
  void add(uint64_t id) {
    plist_.insert(lower_bound(plist_.begin(), plist_.end(), id), id);
  }

  /**
   * Add the identifier of a document to posting list.
   * If the number of stored documents is more than maximum size,
   * the oldest document would be deleted from posting list.
   * @param id the identifier of a document
   * @param max maximum size of posting list
   */
  void add(uint64_t id, size_t max) {
    plist_.insert(lower_bound(plist_.begin(), plist_.end(), id), id);
    if (plist_.size() > max) {
      std::copy(plist_.begin() + plist_.size() - max,
                plist_.end(), plist_.begin());
      plist_.resize(max);
    }
  }

  /**
   * Delete the identifier of a document from posting list.
   * @param id the identifier of a document
   */
  void remove(uint64_t id) {
    std::vector<uint64_t>::iterator it =
      lower_bound(plist_.begin(), plist_.end(), id);
    if (*it == id) plist_.erase(it);
  }

  /**
   * Clear positing list.
   */
  void clear() { plist_.clear(); }

  /**
   * Get the list of the identifiers of stored documents.
   * @param v output list
   */
  void list(std::vector<uint64_t> &v) const {
    std::copy(plist_.begin(), plist_.end(), back_inserter(v));
  }

  /**
   * Check whether posting list is empty or not.
   * @return if empty return true
   */
  bool empty() const { return plist_.empty(); }

  /**
   * Save posting list to a file.
   * @param ofs output stream
   */
  void save(std::ofstream &ofs) const {
    size_t size = plist_.size();
    ofs.write((const char *)&size, sizeof(size));
    if (size > 0) {
      ofs.write((const char *)&plist_[0], sizeof(plist_[0]) * size);
    }
  }

  /**
   * Load posting list from a file.
   * @param ifs input stream
   */
  void load(std::ifstream &ifs) {
    clear();
    size_t size;
    ifs.read((char *)&size, sizeof(size));
    if (size > 0) {
      plist_.resize(size);
      ifs.read((char *)&plist_[0], sizeof(plist_[0]) * size);
    }
  }
};


/**
 * Posting list using Variable Byte code.
 */
class VarBytePostingList {
 private:
  char *plist_;  ///< compressed list of document ids

 public:
  /** Constructor */
  VarBytePostingList() { plist_ = NULL; }

  /** Destructor */
  ~VarBytePostingList() { if (plist_) delete [] plist_; }

  /**
   * Add the identifier of a document id.
   * @param id the identifier of a document
   */
  void add(uint64_t id) {
    std::vector<uint64_t> v;
    if (plist_) {
      decompress_diff(plist_, v);
      v.insert(lower_bound(v.begin(), v.end(), id), id);
      delete [] plist_;
    } else {
      v.push_back(id);
    }
    plist_ = compress_diff(v);
  }

  /**
   * Add the identifier of a document to posting list.
   * If the number of stored documents is more than maximum size,
   * the oldest document would be deleted from posting list.
   * @param id the identifier of a document
   * @param max maximum size of posting list
   */
  void add(uint64_t id, size_t max) {
    std::vector<uint64_t> v;
    if (plist_) {
      decompress_diff(plist_, v);
      v.insert(lower_bound(v.begin(), v.end(), id), id);
      delete [] plist_;
    } else {
      v.push_back(id);
    }
    plist_ = (v.size() > max)
      ? compress_diff(v.begin() + v.size() - max, v.end())
      : compress_diff(v);
  }

  /**
   * Delete the identifier of a document from posting list.
   * @param id the identifier of a document
   */
  void remove(uint64_t id) {
    if (!plist_) return;
    std::vector<uint64_t> v;
    decompress_diff(plist_, v);
    std::vector<uint64_t>::iterator dit = lower_bound(v.begin(), v.end(), id);
    if (*dit == id) {
      v.erase(dit);
      delete [] plist_;
      plist_ = (v.size() > 0) ? compress_diff(v) : NULL;
    }
  }

  /**
   * Clear positing list.
   */
  void clear() {
    if (plist_) {
      delete [] plist_;
      plist_ = NULL;
    }
  }

  /**
   * Get the list of the identifiers of stored documents.
   * @param v output list
   */
  void list(std::vector<uint64_t> &v) const {
    if (plist_) decompress_diff(plist_, v);
  }

  /**
   * Check whether posting list is empty or not.
   * @return if empty return true
   */
  bool empty() const {
    return plist_ ? false : true;
  }

  /**
   * Save posting list to a file.
   * @param ofs output stream
   */
  void save(std::ofstream &ofs) const {
    if (plist_) {
      size_t size = sizeof_compressed(plist_);
      ofs.write((const char *)&size, sizeof(size));
      ofs.write((const char *)plist_, size);
    }
  }

  /**
   * Load posting list from a file.
   * @param ifs input stream
   */
  void load(std::ifstream &ifs) {
    clear();
    size_t size;
    ifs.read((char *)&size, sizeof(size));
    plist_ = new char[size];
    ifs.read((char *)plist_, size);
  }
};

/**
 * Posting list using pfor delta compression.
 *
 * No implementation yet.
 */
class PforPostingList {
 public:
  /**
   * Add the identifier of a document id.
   * @param id the identifier of a document
   */
  void add(uint64_t id) {
  }
  /**
   * Delete the identifier of a document from posting list.
   * @param id the identifier of a document
   */
  void remove(uint64_t id) {
  }
  /**
   * Clear positing list.
   */
  void clear() {
  }
  /**
   * Get the list of the identifiers of stored documents.
   * @param v output list
   */
  void list(std::vector<uint64_t> &v) const {
  }
  /**
   * Check whether posting list is empty or not.
   * @return if empty return true
   */
  bool empty() const {
    return false;
  }
  /**
   * Save posting list to a file.
   * @param ofs output stream
   */
  void save(std::ofstream &ofs) const {
  }
  /**
   * Load posting list from a file.
   * @param ifs input stream
   */
  void load(std::ifstream &ifs) {
  }
};

} /* namespace stupa */

#endif  // STUPA_POSTING_LIST_H_
