//
// Utility functions
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

#ifndef STUPA_UTIL_H
#define STUPA_UTIL_H

#include <stdint.h>
#include <sys/time.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "config.h"

/* include hash_map header */
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
#include <google/dense_hash_map>
#elif HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#else
#include <map>
#endif

/* isnan for win32 */
#ifdef _WIN32
#include <cfloat>
#define isnan(x) _isnan(x)
#endif

/**
 * Hash function of string key for __gnu_cxx::hash_map.
 */
#if !defined(HAVE_GOOGLE_DENSE_HASH_MAP) && defined(HAVE_EXT_HASH_MAP)
namespace __gnu_cxx {
  template<> struct hash<std::string> {
    size_t operator() (const std::string &x) const {
      return hash<const char *>()(x.c_str());
    }
  };
}
#endif


namespace stupa {

/**
 * Type definistion of hash map, 
 * 'google::dense_hash_map' or '__gnu__cxx::hash_map' or 'std::map'.
 */
template<typename KeyType, typename ValueType>
struct HashMap {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
  typedef google::dense_hash_map<KeyType, ValueType> type;
#elif HAVE_EXT_HASH_MAP
  typedef __gnu_cxx::hash_map<KeyType, ValueType> type;
#else
  typedef std::map<KeyType, ValueType> type;
#endif
};

const unsigned int DEFAULT_SEED = 12345;  ///< default seed value
const std::string DELIMITER("\t");        ///< delimiter string

/**
 * Initialize hash_map object (for google::dense_hash_map).
 * @param empty_key key of empty entry
 * @param hmap hash_map object
 */
template<typename KeyType, typename HashType>
void init_hash_map(const KeyType &empty_key, HashType &hmap) {
#ifdef HAVE_GOOGLE_DENSE_HASH_MAP
  hmap.max_load_factor(0.9);
  hmap.set_empty_key(empty_key);
#endif
}

/**
 * Compare pair items.
 * @param left  item
 * @param right item
 * @return return true if left_value > right_value
 */
template<typename KeyType, typename ValueType>
bool greater_pair(const std::pair<KeyType, ValueType> &left,
                  const std::pair<KeyType, ValueType> &right) {
  if (left.second > right.second) {
    return true;
  } else if (left.second == right.second) {
    return left.first > right.first;
  } else {
    return false;
  }
}

/**
 * Compare pair items by absolute value.
 * @param left  item
 * @param right item
 * @return return true if abs(left_value) > abs(right_value)
 */
template<typename KeyType, typename ValueType>
bool greater_pair_abs(const std::pair<KeyType, ValueType> &left,
                      const std::pair<KeyType, ValueType> &right) {
  ValueType absleft = std::abs(left.second);
  ValueType absright = std::abs(right.second);
  if (absleft > absright) {
    return true;
  } else if (absleft == absright) {
    return left.first > right.first;
  } else {
    return false;
  }
}

/**
 * Set seed for random number generator.
 * @param seed seed
 */
void mysrand(unsigned int seed);

/**
 * Get random number.
 * @param seed pointer of seed
 * @return random number
 */
int myrand(unsigned int *seed);

/**
 * Delta compression.
 * @param v input array of integer
 * @return compressed data
 */
char *compress_diff(const std::vector<uint64_t> &v);

/**
 * Delta compression.
 * @param begin beginning iterator of input array of integer
 * @param end end iterator of input array of integer
 * @return compressed data
 */
char *compress_diff(const std::vector<uint64_t>::iterator &begin,
                    const std::vector<uint64_t>::iterator &end);

/**
 * Delta decompression.
 * @param ptr compressed data
 * @param v output array of integer
 */
void decompress_diff(const char *ptr, std::vector<uint64_t> &v);

/**
 * Get size of compressed data.
 * @param ptr compressed data
 * @return size of array of integer
 */
size_t sizeof_compressed(const char *ptr);

/**
 * Get current time.
 * @return current time
 */
double get_time();

/**
 * Get random ASCII string.
 * @param max max size of output string
 * @param result output string
 */
void random_string(size_t max, std::string &result);

/**
 * Get file extension.
 * @param filename file name
 * @return extension
 */
std::string get_extension(const std::string filename);

/**
 * Split a string by delimiter string
 * @param s input string to be splited
 * @param delimiter delimiter string
 * @param splited splited strings
 */
void split_string(const std::string &s, const std::string &delimiter,
                  std::vector<std::string> &splited);


/**
 * Random number generator class.
 */
class Random {
 private:
  unsigned int seed_;  ///< seed of a random number generator

 public:
  /**
   * Default Constructor.
   */
  Random() : seed_(DEFAULT_SEED) { }
  /**
   * Constructor.
   * @param seed seed of a random number generator
   */
  explicit Random(unsigned int seed) : seed_(seed) { }
  /**
   * Deconstructor.
   */
  ~Random() { }
  /**
   * Set seed value.
   * @param seed seed of a random number generator
   */
  void set_seed(unsigned int seed) { seed_ = seed; }
  /**
   * Get a random number.
   * @param max max number of output random number
   * @return a random number
   */
  unsigned int operator()(unsigned int max) {
    double ratio = static_cast<double>(myrand(&seed_))
                   / static_cast<double>(RAND_MAX);
    return static_cast<unsigned int>(ratio * max);
  }
};

} /* namespace stupa */

#endif  // STUPA_UTIL_H
