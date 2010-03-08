//
// Stupa Search command line tool
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

#include <cstdio>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "stupa.h"

/** Default value of maximum size of inverted indexes */
const size_t DEFAULT_INV_SIZE = 100;
const std::string PROMPT("Query> ");

// function prototypes
int main(int argc, char **argv);
static void usage(const char *progname);
static void load_file(stupa::StupaSearch &stpsearch,
                      const char *path, bool is_binary, size_t invsize);
static int run_search(int argc, char **argv);
static int run_save(int argc, char **argv);

int main(int argc, char **argv) {
  if (argc < 2) usage(argv[0]);
  std::string command(argv[1]);
  if (command == "search") {
    return run_search(argc, argv);
  } else if (command == "save") {
    return run_save(argc, argv);
  } else {
    usage(argv[0]);
  }
}

static void load_file(stupa::StupaSearch &stpsearch,
                      const char *path, bool is_binary, size_t invsize) {
  std::ifstream ifs(path);
  if (!ifs) {
    fprintf(stderr, "[ERROR]Cannot open file: %s\n", path);
    std::exit(EXIT_FAILURE);
  }
  if (is_binary) {
    printf("Reading input documents (Binary, invsize:ignored) ... ");
    fflush(stdout);
    stpsearch.load(ifs);
  } else {
    printf("Reading input documents (Text, invsize:%d) ... ",
           static_cast<int>(invsize));
    fflush(stdout);
    stpsearch.read_tsvfile(ifs);
  }
  printf("%d documents\n", static_cast<int>(stpsearch.size()));
}

/**
 * Show usage.
 * @param progname name of this program
 */
static void usage(const char *progname) {
  fprintf(stderr, "%s: Stupa Search utility\n\n", progname);
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, " %% %s search [-b][-f] file [invsize]\n", progname);
  fprintf(stderr, " %% %s save [-b] infile outfile [invsize]\n", progname);
  fprintf(stderr, "    -b        read binary format file\n");
  fprintf(stderr, "    -f        search by feature strings\n");
  fprintf(stderr, "              (default: search by document identifier strings)\n");
  fprintf(stderr, "    invsize   maximum size of inverted indexes (default:%d)\n",
          static_cast<int>(DEFAULT_INV_SIZE));
  std::exit(EXIT_FAILURE);
}

/**
 * Search related documents.
 * @param argc the number of arguments
 * @param argv argument strings
 */
static int run_search(int argc, char **argv) {
  const char *progname = argv[0];
  if (argc < 3) usage(progname);
  bool is_binary = false;
  bool by_feature = false;
  const char *path = NULL;
  size_t invsize = 0;
  for (int i = 2; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (!strcmp(argv[i], "-b")) {
        is_binary = true;
      } else if (!strcmp(argv[i], "-f")) {
        by_feature = true;
      }
    } else if (!path) {
      path = argv[i];
    } else if (!invsize) {
      invsize = atoi(argv[i]);
    } else {
      usage(progname);
    }
  }
  if (!path) usage(progname);
  if (invsize <= 0) invsize = DEFAULT_INV_SIZE;
  stupa::StupaSearch stpsearch(stupa::SearchModel::INNER_PRODUCT, invsize);
  load_file(stpsearch, path, is_binary, invsize);

  std::vector<std::string> queries;
  std::vector<std::pair<std::string, stupa::Point> > results;
  std::string line;
  printf("%s", PROMPT.c_str());
  while (std::getline(std::cin, line)) {
    stupa::split_string(line, "\t", queries);
    double start = stupa::get_time();
    if (by_feature) {
      stpsearch.search_by_feature(queries, results);
    } else {
      stpsearch.search_by_document(queries, results);
    }
    double search_time = stupa::get_time() - start;
    for (size_t i = 0; i < results.size(); i++) {
      printf("%s%s%f\n", results[i].first.c_str(),
             stupa::DELIMITER.c_str(), results[i].second);
    }
    printf("(search time: %.2fms)\n\n", search_time * 1000);
    queries.clear();
    results.clear();
    printf("%s", PROMPT.c_str());
  }
  return EXIT_SUCCESS;
}

/**
 * Save documents and inverted indexes, ... to a file.
 * @param argc the number of arguments
 * @param argv argument strings
 */
static int run_save(int argc, char **argv) {
  const char *progname = argv[0];
  if (argc < 4) usage(progname);
  bool is_binary = false;
  const char *inpath = NULL;
  const char *outpath = NULL;
  size_t invsize = 0;
  for (int i = 2; i < argc; i++) {
    if (!strcmp(argv[i], "-b")) {
      is_binary = true;
    } else if (!inpath) {
      inpath = argv[i];
    } else if (!outpath) {
      outpath = argv[i];
    } else if (!invsize) {
      invsize = atoi(argv[i]);
    } else {
      usage(progname);
    }
  }
  if (!inpath || !outpath) usage(progname);

  std::ofstream ofs(outpath);
  if (!ofs) {
    fprintf(stderr, "[ERROR]Cannot open file: %s\n", outpath);
    return EXIT_FAILURE;
  }
  if (invsize <= 0) invsize = DEFAULT_INV_SIZE;
  stupa::StupaSearch stpsearch(stupa::SearchModel::INNER_PRODUCT, invsize);
  load_file(stpsearch, inpath, is_binary, invsize);
  printf("Writing data to the output file ... ");
  fflush(stdout);
  stpsearch.save(ofs);
  printf("finished\n");
  return EXIT_SUCCESS;
}
