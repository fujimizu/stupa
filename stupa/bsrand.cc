//
// Bayesian Sets load-test tool using randomly generated data
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
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <map>
#include <vector>
#include "stupa.h"
#include <sstream>

/**
 * Load-test setting.
 */
struct Setting {
  uint64_t dnum;  ///< the number of documents
  uint64_t fnum;  ///< the number of features of each document
  uint64_t qnum;  ///< the number of queries
  uint64_t isiz;  ///< maximum size of posting list of inverted index

  /**
   * Check setting parameters.
   */
  void check() {
    if (dnum <= 0 || fnum <= 0 || qnum <= 0 || isiz < 0) {
      fprintf(stderr, "[ERROR]dnum/fnum/qnum/isiz must be more than zero.\n");
      exit(1);
    }
    if (dnum < qnum) {
      fprintf(stderr, "[ERROR]qnum must be less than dnum.");
      exit(1);
    }
  }
  /**
   * Set setting parameters.
   */
  void set(char **argv) {
    char *ptr;
    dnum = strtoull(argv[0], &ptr, 10);
    fnum = strtoull(argv[1], &ptr, 10);
    qnum = strtoull(argv[2], &ptr, 10);
    isiz = strtoull(argv[3], &ptr, 10);
    check();
  }
  /**
   * Show setting parameters.
   */
  void show() {
    printf("[Load-test Setting]\n");
    printf(" number of documents                        = %lld\n",
           static_cast<unsigned long long>(dnum));
    printf(" number of the features of each document    = %lld\n",
           static_cast<unsigned long long>(fnum));
    printf(" number of search queries                   = %lld\n",
           static_cast<unsigned long long>(qnum));
    printf(" max size of posting list of inverted index = %lld\n\n",
           static_cast<unsigned long long>(isiz));
  }
};

const size_t MAX_RESULT = 20;   ///< maximum number of search results
const size_t NUM_LOOP   = 100;  ///< the number of times searching
const size_t STR_LENGTH = 10;   ///< maximum length of a feature string

// function prototypes
int main(int argc, char **argv);
static void usage(const char *progname);
static uint64_t zipf_rand(uint64_t nkinds, uint64_t min);

/**
 * Abstract class for load test.
 */
class LoadTest {
 private:
  /**
   * Set data for load test.
   */
  virtual void set_testset() = 0;
  /**
   * Do searching for some times.
   */
  virtual void search_loop() = 0;
  /**
   * Do adding a document for some times.
   */
  virtual void add_loop() = 0;
  /**
   * Do deleting a document for some times.
   */
  virtual void delete_loop() = 0;

  /**
   * Show load-test result.
   * @param search_time time spent on searching loop
   * @param add_time time spent on adding time
   * @param delete_time time spent on deleting time
   */
  void show_result(double search_time, double add_time,
                   double delete_time) const {
    printf("[Load-test Result]\n");
    printf(" Search : %.2f (qry/sec)\n", NUM_LOOP / search_time);
    printf(" Add    : %.2f (add/sec)\n", NUM_LOOP / add_time);
    printf(" Delete : %.2f (del/sec)\n", NUM_LOOP / delete_time);
  }

 protected:
  Setting setting_;  ///< load-test setting

 public:
  /**
   * Constructor.
   * @param setting load-test setting
   */
  explicit LoadTest(const Setting &setting) : setting_(setting) { }
  /**
   * Destructor.
   */
  virtual ~LoadTest() { }

  /**
   * Execute load-test and show result.
   */
  void execute() {
    printf(" Setting inputs ... ");
    fflush(stdout);
    set_testset();
    printf("done\n");

    double start, search_time, add_time, delete_time;

    printf(" Search loop    ... ");
    fflush(stdout);
    start = stupa::get_time();
    search_loop();
    search_time = stupa::get_time() - start;
    printf("done\n");

    printf(" Add loop       ... ");
    fflush(stdout);
    start = stupa::get_time();
    add_loop();
    add_time = stupa::get_time() - start;
    printf("done\n");

    printf(" Delete loop    ... ");
    fflush(stdout);
    start = stupa::get_time();
    delete_loop();
    delete_time = stupa::get_time() - start;
    printf("done\n\n");

    show_result(search_time, add_time, delete_time);
  }
};

/**
 * Load test class using identifiers of integers.
 */
class LoadTestId : public LoadTest {
 private:
  /**
   * Type definition of <document id, list of feature id> map
   */
  typedef stupa::HashMap<stupa::DocumentId,
                       std::vector<stupa::FeatureId> *>::type TestSetId;

  TestSetId ts_;              ///< test data
  stupa::BayesianSets bs_;    ///< bayesian sets
  stupa::InvertedIndex inv_;  ///< inverted index

  /**
   * Set random features.
   * @param features output features to be set random numbers
   */
  void random_features(std::vector<stupa::FeatureId> &features) const {
    std::map<stupa::FeatureId, bool> check;
    uint64_t cnt = 0;
    while (cnt < setting_.fnum) {
      //stupa::FeatureId fid = rand() % setting_.dnum + stupa::FEATURE_START_ID;
      stupa::FeatureId fid = zipf_rand(setting_.dnum, stupa::FEATURE_START_ID);
      if (check.find(fid) == check.end()) {
        features.push_back(fid);
        check[fid] = true;
        cnt++;
      }
    }
    std::sort(features.begin(), features.end());
  }

  /**
   * Set random queries.
   * @param queries output queries to be set random document ids
   */
  void random_queries(std::vector<stupa::DocumentId> &queries) const {
    std::map<stupa::DocumentId, bool> check;
    check[stupa::DOC_EMPTY_ID] = true;
    check[stupa::DOC_DELETED_ID] = true;
    uint64_t cnt = 0;
    while (cnt < setting_.qnum) {
      stupa::DocumentId did =
        static_cast<stupa::DocumentId>(rand()) % setting_.dnum
        + stupa::DOC_START_ID;
      if (check.find(did) == check.end()) {
        queries.push_back(did);
        check[did] = true;
        cnt++;
      }
    }
  }

  /**
   * Look up inverted indexes.
   * @param queries search queries
   * @param results list of document ids
   */
  void lookup_inverted_index(const std::vector<stupa::DocumentId> &queries,
                             std::vector<stupa::DocumentId> &result) {
    stupa::HashMap<stupa::FeatureId, bool>::type fidmap;
    stupa::init_hash_map(stupa::FEATURE_EMPTY_ID, fidmap);
    std::vector<stupa::FeatureId> feature_ids;
    for (size_t i = 0; i < queries.size(); i++) {
      bs_.feature(queries[i], feature_ids);
      for (size_t i = 0; i < feature_ids.size(); i++) {
        fidmap[feature_ids[i]] = true;
      }
      feature_ids.clear();
    }
    stupa::HashMap<stupa::FeatureId, bool>::type::iterator it;
    for (it = fidmap.begin(); it != fidmap.end(); ++it) {
      feature_ids.push_back(it->first);
    }
    inv_.lookup(feature_ids, result);
  }

  /**
   * Show size of posting list.
   */
  void show_posting_size() const {
    uint64_t sum = 0;
    std::vector<stupa::DocumentId> list;
    stupa::InvertedIndex::IndexHash::const_iterator it;
    for (it = inv_.index().begin(); it != inv_.index().end(); ++it) {
      it->second->list(list);
      printf("%lld\t%lld\n",
             static_cast<unsigned long long>(it->first),
             static_cast<unsigned long long>(list.size()));
      sum += list.size();
      list.clear();
    }
    printf("%f\n", static_cast<double>(sum) / inv_.size());
  }

  /**
   * Set data for load test.
   */
  void set_testset() {
    stupa::DocumentId did = stupa::DOC_START_ID;
    while (did++ < setting_.dnum + stupa::DOC_START_ID) {
      std::vector<stupa::FeatureId> features;
      random_features(features);
      bs_.add_document(did, features);
      inv_.add_document(did, features);
    }
    while (did++ < setting_.dnum + stupa::DOC_START_ID + NUM_LOOP) {
      std::vector<stupa::FeatureId> *features =
        new std::vector<stupa::FeatureId>;
      random_features(*features);
      ts_[did] = features;
    }
  }

  /**
   * Do searching for some times.
   */
  void search_loop() {
    std::vector<stupa::DocumentId> queries;
    std::vector<stupa::DocumentId> candidates;
    std::vector<std::pair<stupa::DocumentId, stupa::Point> > results;
    for (size_t i = 0; i < NUM_LOOP; i++) {
      queries.clear();
      candidates.clear();
      results.clear();
      random_queries(queries);
      lookup_inverted_index(queries, candidates);
      bs_.search_by_document(queries, candidates, results, MAX_RESULT);
    }
  }

  /**
   * Do adding a document for some times.
   */
  void add_loop() {
    for (TestSetId::const_iterator it = ts_.begin(); it != ts_.end(); ++it) {
      bs_.add_document(it->first, *it->second);
      inv_.add_document(it->first, *it->second);
    }
  }

  /**
   * Do deleting a document for some times.
   */
  void delete_loop() {
    std::vector<stupa::FeatureId> features;
    for (TestSetId::const_iterator it = ts_.begin(); it != ts_.end(); ++it) {
      bs_.feature(it->first, features);
      bs_.delete_document(it->first);
      inv_.delete_document(it->first, features);
      features.clear();
    }
  }

 public:
  /**
   * Constructor.
   * @param setting load-test setting
   */
  explicit LoadTestId(const Setting &setting)
    : LoadTest(setting), inv_(setting.isiz) {
    stupa::init_hash_map(stupa::DOC_EMPTY_ID, ts_);
  }

  /**
   * Destructor.
   */
  ~LoadTestId() {
    for (TestSetId::iterator it = ts_.begin(); it != ts_.end(); ++it) {
      if (it->second) delete it->second;
    }
  }
};

/**
 * Load test class using identifiers of strings.
 */
class LoadTestText : public LoadTest {
 private:
  /**
   * Type definition of <string identifier, list of feature strings> map
   */
  typedef stupa::HashMap<std::string, std::vector<std::string> *>::type TestSetText;

  TestSetText ts_;                      ///< test data
  stupa::BayesianSetsSearch bssearch_;  ///< search
  std::vector<std::string> documents_;  ///< identifier string of documents

  /**
   * Set random features.
   * @param features output features to be set random strings
   */
  void random_features(std::vector<std::string> &features) const {
    std::stringstream ss;
    std::map<uint64_t, bool> check;
    size_t cnt = 0;
    while (cnt < setting_.fnum) {
      uint64_t n = rand() % setting_.dnum + stupa::FEATURE_START_ID;
      // uint64_t n = zipf_rand(setting_.dnum, stupa::FEATURE_START_ID);
      if (check.find(n) == check.end()) {
        ss << n;
        features.push_back(ss.str());
        check[n] = true;
        cnt++;
      }
      ss.str("");
    }
  }

  /**
   * Set data for load test.
   */
  void set_testset() {
    stupa::DocumentId did = 0;
    std::map<std::string, bool> check;
    while (did < setting_.dnum + NUM_LOOP) {
      std::string didstr;
      do {
        /*
        std::stringstream ss;
        ss << did;
        didstr = ss.str();
        */
        stupa::random_string(STR_LENGTH, didstr);
      } while (check.find(didstr) != check.end());
      check[didstr] = true;
      std::vector<std::string> *features = new std::vector<std::string>;
      random_features(*features);
      if (did < setting_.dnum) {
        bssearch_.add_document(didstr, *features);
        documents_.push_back(didstr);
        delete features;
      } else {
        ts_[didstr] = features;
      }
      did++;
    }
  }

  /**
   * Do searching for some times.
   */
  void search_loop() {
    std::map<size_t, bool> check;
    std::vector<std::string> queries;
    std::vector<std::pair<std::string, stupa::Point> > results;
    for (size_t i = 0; i < NUM_LOOP; i++) {
      results.clear();
      queries.clear();
      check.clear();
      size_t cnt = 0;
      while (cnt < setting_.qnum) {
        size_t index = static_cast<size_t>(rand() % documents_.size());
        if (check.find(index) == check.end()) {
          queries.push_back(documents_[index]);
          check[index] = true;
          cnt++;
        }
      }
      bssearch_.search(queries, results, MAX_RESULT);
    }
  }

  /**
   * Do adding a document for some times.
   */
  void add_loop() {
    for (TestSetText::const_iterator it = ts_.begin();
         it != ts_.end(); ++it) {
      bssearch_.add_document(it->first, *it->second);
    }
  }

  /**
   * Do deleting a document for some times.
   */
  void delete_loop() {
    for (TestSetText::const_iterator it = ts_.begin();
         it != ts_.end(); ++it) {
      bssearch_.delete_document(it->first);
    }
  }

 public:
  /**
   * Constructor.
   * @param setting load-test setting
   */
  explicit LoadTestText(const Setting &setting)
    : LoadTest(setting), bssearch_(setting.isiz) {
    stupa::init_hash_map("", ts_);
  }

  /**
   * Destructor.
   */
  ~LoadTestText() {
    for (TestSetText::iterator it = ts_.begin(); it != ts_.end(); ++it) {
      if (it->second) delete it->second;
    }
  }
};


int main(int argc, char **argv) {
  if (argc < 5) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  srand((unsigned int)time(NULL));
  if (argc == 5) {
    Setting setting;
    setting.set(argv+1);
    setting.show();
    LoadTestId test(setting);
    test.execute();
  } else if (argc == 6 && !strcmp(argv[1], "-text")) {
    Setting setting;
    setting.set(argv+2);
    setting.show();
    LoadTestText test(setting);
    test.execute();
  } else {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

/**
 * Show usage
 * @param progname name of this program
 */
static void usage(const char *progname) {
  fprintf(stderr, "%s : Bayesian Sets load-test tool\n\n", progname);
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "%% %s [-text] dnum fnum qnum isiz\n", progname);
  fprintf(stderr, "     dnum : number of documents\n");
  fprintf(stderr, "     fnum : number of the features of each document\n");
  fprintf(stderr, "     qnum : number of search queries\n");
  fprintf(stderr, "     isiz : max size of posting list of inverted index\n");
}

/**
 * Generate random number based on zipf's law
 * @param nkinds the number of kinds of output number
 * @param min minimum number of output number
 * @return random number
 */
static uint64_t zipf_rand(uint64_t nkinds, uint64_t min) {
  return static_cast<uint64_t>(
    pow(M_E, (static_cast<double>(rand()) / RAND_MAX * log(nkinds + 1.0)))
    - 1.0) + min;
}
