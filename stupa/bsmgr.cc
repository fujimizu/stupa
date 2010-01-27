//
// Bayesian Sets command line tool
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
#include <string>
#include <utility>
#include <vector>
#include "stupa.h"

/** Default value of maximum size of inverted indexes */
const size_t DEFAULT_INV_SIZE = 100;
const std::string PROMPT("query> ");

// function prototypes
int main(int argc, char **argv);
static void usage(const char *progname);
static int run_search(int argc, char **argv);
static int run_save(int argc, char **argv);
static void read_tsv(std::ifstream &ifs, stupa::BayesianSetsSearch &bssearch);

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  std::string command(argv[1]);
  if (command == "search") {
    return run_search(argc, argv);
  } else if (command == "save") {
    return run_save(argc, argv);
  } else {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
}

/**
 * Show usage.
 * @param progname name of this program
 */
static void usage(const char *progname) {
  fprintf(stderr, "%s: Bayesian Sets utility\n\n", progname);
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, " %% %s search file [invsize]  (default_invsize=%d)\n",
          progname, static_cast<int>(DEFAULT_INV_SIZE));
  fprintf(stderr, " %% %s save infile outfile [invsize]\n", progname);
}

/**
 * Search related documents.
 * @param argc the number of arguments
 * @param argv argument strings
 */
static int run_search(int argc, char **argv) {
  if (argc < 3) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  std::ifstream ifs(argv[2]);
  if (!ifs) {
    fprintf(stderr, "[ERROR]Cannot open file: %s\n", argv[2]);
    return EXIT_FAILURE;
  }
  printf("Read input documents ...\n");
  size_t invsize = argc == 4 ?
    static_cast<size_t>(atoi(argv[3])) : DEFAULT_INV_SIZE;
  if (invsize <= 0) {
    fprintf(stderr, "[ERROR]Size of inverted index must be greater than zero.");
    return EXIT_FAILURE;
  }

  stupa::BayesianSetsSearch bssearch(invsize);
  if (stupa::get_extension(argv[2]) == "tsv") {
    read_tsv(ifs, bssearch);
  } else {
    bssearch.load(ifs);
  }
  std::vector<std::string> queries;
  std::vector<std::pair<std::string, stupa::Point> > results;
  std::string line;
  printf("%s", PROMPT.c_str());
  while (std::getline(std::cin, line)) {
    stupa::split_string(line, "\t", queries);
    bssearch.search(queries, results);
    for (size_t i = 0; i < results.size(); i++) {
      printf("%s%s%f\n",
             results[i].first.c_str(), stupa::DELIMITER.c_str(), results[i].second);
    }
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
  if (argc < 4) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  std::ifstream ifs(argv[2]);
  if (!ifs) {
    fprintf(stderr, "[ERROR]Cannot open file: %s\n", argv[2]);
    return EXIT_FAILURE;
  }
  std::ofstream ofs(argv[3]);
  if (!ofs) {
    fprintf(stderr, "[ERROR]Cannot open file: %s\n", argv[3]);
    return EXIT_FAILURE;
  }
  size_t invsize = argc == 5 ?
    static_cast<size_t>(atoi(argv[4])) : DEFAULT_INV_SIZE;
  if (invsize <= 0) {
    fprintf(stderr, "[ERROR]Size of inverted index must be greater than zero.");
    return EXIT_FAILURE;
  }
  stupa::BayesianSetsSearch bssearch(invsize);
  read_tsv(ifs, bssearch);
  bssearch.save(ofs);
  return EXIT_SUCCESS;
}

/**
 * Read input file and add documents to BayesianSets searcher.
 * @param ifs input stream
 * @param bssearch bayesian sets searcher
 */
static void read_tsv(std::ifstream &ifs, stupa::BayesianSetsSearch &bssearch) {
  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      size_t p = line.find(stupa::DELIMITER);
      std::string doc_name = line.substr(0, p);
      line = line.substr(p + stupa::DELIMITER.size());
      std::vector<std::string> features;
      stupa::split_string(line, stupa::DELIMITER, features);
      if (!doc_name.empty() && !features.empty()) {
        bssearch.add_document(doc_name, features);
      }
    }
  }
}
