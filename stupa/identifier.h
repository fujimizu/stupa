//
// Type defitions of identifiers
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

#ifndef STUPA_IDENTIFIER_H_
#define STUPA_IDENTIFIER_H_

#include <stdint.h>

namespace stupa {

/** Type definitions */
typedef uint64_t DocumentId;  ///< type definition of document id
typedef uint64_t FeatureId;   ///< type definition of feature id
typedef double   Point;       ///< type definition of point of vectors
typedef char *   Feature;     ///< type definition of compressed features

/** Constants */
const FeatureId  FEATURE_EMPTY_ID   = 0;  ///< empty_key of feature id
const FeatureId  FEATURE_DELETED_ID = 1;  ///< deleted_key of feature id
const FeatureId  FEATURE_START_ID   = 2;  ///< start number of feature id
const DocumentId DOC_EMPTY_ID       = 0;  ///< empty_key of document id
const DocumentId DOC_DELETED_ID     = 1;  ///< deleted_key of document id
const DocumentId DOC_START_ID       = 2;  ///< start number of document id

}  /* namespace stupa */

#endif  // STUPA_IDENTIFIER_H_
