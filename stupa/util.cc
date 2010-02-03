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

#include <sys/time.h>
#include <cstdlib>
#include "util.h"

namespace {
/** characters for random string generation. */
const std::string CHARACTERS(
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz123456789");
} /* namespace */

namespace stupa {

/**
 * Set seed for random number generator.
 */
void mysrand(unsigned int seed) {
#if (_POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
  // do nothing
#else
  srand(seed);
#endif
}

/**
 * Get random number.
 */
int myrand(unsigned int *seed) {
#if (_POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
  return rand_r(seed);
#else
  return rand();
#endif
}

/**
 * Encode an integer by Variable Byte code.
 * @param num input integer to be encoded
 * @param ptr output encoded data
 * @param size allocated size of output encoded data
 */
static void variable_byte_encode_number(uint64_t num, char *ptr, int size) {
  char *end = ptr + size - 1;
  for (;;) {
    (*end--) = num % 128;
    if (num < 128) break;
    num /= 128;
  }
  *(ptr+size-1) += 128;
}

/**
 * Encode an array of integers by Variable Byte code.
 * @param v input array of integer to be encoded
 * @param total_size total allocated size of output encoded data
 * @param sizes allocated sizes of encoded data of each integer
 * @return encoded data, must be deleted later
 */
static char *variable_byte_encode(const std::vector<uint64_t> &v,
                                  int total_size,
                                  const std::vector<int> &sizes) {
  char *buf = new char[total_size];
  char *ptr = buf;
  for (size_t i = 0; i < v.size(); i++) {
    variable_byte_encode_number(v[i], ptr, sizes[i]);
    ptr += sizes[i];
  }
  return buf;
}

/**
 * Decode data by Variable Byte code.
 * @param ptr encoded data to be decoded
 * @param v output array of integers
 * @return the number of decoded array of integers
 */
static size_t variable_byte_decode(const char *ptr, std::vector<uint64_t> &v) {
  size_t byte_size = 0;
  uint64_t siz = 0;
  uint64_t c = *(unsigned char *)ptr++;
  byte_size++;
  while (c < 128) {
    siz = 128 * siz + c;
    c = *(unsigned char *)ptr++;
    byte_size++;
  }
  siz = 128 * siz + (c - 128);

  uint64_t cnt = 0;
  while (cnt < siz) {
    uint64_t n = 0;
    c = *(unsigned char *)ptr++;
    byte_size++;
    while (c < 128) {
      n = 128 * n + c;
      c = *(unsigned char *)ptr++;
      byte_size++;
    }
    n = 128 * n + (c - 128);
    v.push_back(n);
    cnt++;
  }
  return byte_size;
}

/**
 * Delta compression.
 */
char *compress_diff(const std::vector<uint64_t> &v) {
  if (v.empty()) return NULL;
  std::vector<uint64_t> differences(v.size()+1);
  differences[0] = v.size();
  std::vector<int> sizes(v.size()+1);
  int total_size = 0;

  uint64_t siz = v.size();
  do {
    siz >>= 7;
    sizes[0]++;
  } while (siz);
  total_size += sizes[0];

  uint64_t prev = 0;
  for (size_t i = 0; i < v.size(); i++) {
    uint64_t diff = v[i] - prev;
    prev = v[i];
    differences[i+1] = diff;
    sizes[i+1] = 0;
    do {
      diff >>= 7;
      sizes[i+1]++;
    } while (diff);
    total_size += sizes[i+1];
  }
  return variable_byte_encode(differences, total_size, sizes);
}


/**
 * Delta compression.
 */
char *compress_diff(const std::vector<uint64_t>::iterator &begin,
                    const std::vector<uint64_t>::iterator &end) {
  if (begin == end) return NULL;
  std::vector<uint64_t> differences;
  std::vector<int> sizes;
  differences.push_back(0);
  sizes.push_back(0);
  int total_size = 0;

  uint64_t prev = 0;
  size_t idx = 0;
  for (std::vector<uint64_t>::const_iterator it = begin; it != end; ++it) {
    uint64_t diff = *it - prev;
    prev = *it;
    differences.push_back(diff);
    sizes.push_back(0);
    idx++;
    do {
      diff >>= 7;
      sizes[idx]++;
    } while (diff);
    total_size += sizes[idx];
  }

  differences[0] = idx;
  do {
    idx >>= 7;
    sizes[0]++;
  } while (idx);
  total_size += sizes[0];

  return variable_byte_encode(differences, total_size, sizes);
}

/**
 * Delta decompression.
 */
void decompress_diff(const char *ptr, std::vector<uint64_t> &v) {
  variable_byte_decode(ptr, v);
  for (size_t i = 1; i < v.size(); i++) {
    v[i] += v[i-1];
  }
}

/**
 * Get size of compressed data.
 */
size_t sizeof_compressed(const char *ptr) {
  std::vector<uint64_t> v;
  return variable_byte_decode(ptr, v);
}

/**
 * Get current time.
 */
double get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + static_cast<double>(tv.tv_usec) * 1e-6;
}

/**
 * Get random ASCII string
 */
void random_string(size_t max, std::string &result) {
  size_t size = static_cast<size_t>(rand() % max);
  if (size == 0) size = 1;
  result.resize(size);
  for (size_t i = 0; i < size; i++) {
    int index = static_cast<int>(rand() % CHARACTERS.size());
    result[i] = CHARACTERS[index];
  }
}

/**
 * Get file extention
 */
std::string get_extension(const std::string filename) {
  size_t index = filename.rfind('.', filename.size());
  if (index != std::string::npos) {
    return filename.substr(index+1, filename.size()-1);
  }
  return "";
}

/**
 * Split a string by delimiter string
 */
void split_string(const std::string &s, const std::string &delimiter,
                  std::vector<std::string> &splited) {
  for (size_t i = s.find_first_not_of(delimiter); i != std::string::npos; ) {
    size_t j = s.find_first_of(delimiter, i);
    if (j != std::string::npos) {
      splited.push_back(s.substr(i, j-i));
      i = s.find_first_not_of(delimiter, j+1);
    } else {
      splited.push_back(s.substr(i, s.size()));
      break;
    }
  }
}


} /* namespace stupa */
