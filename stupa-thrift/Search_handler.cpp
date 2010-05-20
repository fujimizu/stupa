//
// Search server implementation with Thrift
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

#include <cstring>
#include "Search_handler.h"

namespace stupa { namespace thrift { /* namespace stupa::thrift */

void usage(const char *progname) {
  fprintf(stderr, "Usage: %s [options]\n", progname);
  fprintf(stderr, " -p port     port number (default:%d)\n", PORT);
  fprintf(stderr, " -d num      maximum number of documents (default: no limit)\n");
  fprintf(stderr, " -i size     maximum size of inverted indexes (default:%d)\n",
          static_cast<int>(INV_SIZE));
  fprintf(stderr, " -w nworker  number of worker thread (default:%d)\n",
          WORKER_COUNT);
  fprintf(stderr, " -f file     load a file (binary format)\n");
  fprintf(stderr, " -h          show help message\n");
  exit(1);
}

void parse_options(int argc, char **argv, ServerParam &param) {
  int i = 1;
  while (i < argc) {
    if (!strcmp(argv[i], "-p")) {
      param.port = atoi(argv[++i]);
      ++i;
    } else if (!strcmp(argv[i], "-d")) {
      param.max_doc = atoi(argv[++i]);
      ++i;
    } else if (!strcmp(argv[i], "-i")) {
      param.invsize = atoi(argv[++i]);
      ++i;
    } else if (!strcmp(argv[i], "-w")) {
      param.workerCount = atoi(argv[++i]);
      ++i;
    } else if (!strcmp(argv[i], "-f")) {
      param.filename = argv[++i];
      ++i;
    } else if (!strcmp(argv[i], "-h")) {
      usage(argv[0]);
    } else {
      usage(argv[0]);
    }
  }
}

}}  /* namespace stupa::thrift */
