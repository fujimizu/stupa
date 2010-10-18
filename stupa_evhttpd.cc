//
// Search server implementation with libevent
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

#include <sys/queue.h>
#include <csignal>
#include <iostream>
#include <event.h>
#include <evhttp.h>
#include "handler.h"
#include "thread.h"

const int PORT          = 8080;
const size_t INV_SIZE   = 100;
const size_t NUM_WORKER = 4;
const size_t MAX_RESULT = 50;

/**
 * Parameters to start stupa server
 */
struct CommandParam {
  int    port;        ///< port number.
  size_t max_doc;     ///< maximum number of documents.
  size_t invsize;     ///< maximum size of inverted indexes.
  size_t num_worker;  ///< the number of worker threads
  char   *filename;   ///< path of input file.

  CommandParam() : port(PORT), max_doc(0), invsize(INV_SIZE),
                   num_worker(NUM_WORKER), filename(NULL) { }
};

/**
 * Request type
 */
enum RequestType {
  ADD,
  DELETE,
  SIZE,
  FSEARCH,
  DSEARCH,
  SAVE,
  LOAD
};

struct CallbackParam {
  stupa::StupaSearchHandler *handler;
  stupa::ThreadPool *pool;
  CallbackParam(stupa::StupaSearchHandler *h, stupa::ThreadPool *p)
    : handler(h), pool(p) { }
};

struct WorkerParam {
  stupa::StupaSearchHandler *handler;
  stupa::ThreadPool *pool;
  evhttp_request *req;
  RequestType type;
  WorkerParam(stupa::StupaSearchHandler *h, stupa::ThreadPool *p,
              evhttp_request *r, RequestType t)
    : handler(h), pool(p), req(r), type(t) { }
};

/* function prototypes */
void usage(const char *progname);
void parse_options(int argc, char **argv, CommandParam &param);
evbuffer *create_buffer(evhttp_request *req);
void write_search_result(
  const std::vector<std::pair<std::string, double> > &results, evbuffer *buf);
void append_job(evhttp_request *req, void *arg, RequestType type);

void cb_add(evhttp_request *req, void *arg);
void cb_delete(evhttp_request *req, void *arg);
void cb_size(evhttp_request *req, void *arg);
void cb_dsearch(evhttp_request *req, void *arg);
void cb_fsearch(evhttp_request *req, void *arg);
void cb_save(evhttp_request *req, void *arg);
void cb_load(evhttp_request *req, void *arg);

void add_handler(WorkerParam *wparam);
void delete_handler(WorkerParam *wparam);
void size_handler(WorkerParam *wparam);
void dsearch_handler(WorkerParam *wparam);
void fsearch_handler(WorkerParam *wparam);
void save_handler(WorkerParam *wparam);
void load_handler(WorkerParam *wparam);

void *worker_func(void *arg);
void start_server(const CommandParam &param);

/**
 * Main function.
 */
int main(int argc, char **argv) {
  CommandParam param;
  parse_options(argc, argv, param);
  start_server(param);
  return EXIT_SUCCESS;
}

/**
 * Show a usage.
 * @param progname program name
 */
void usage(const char *progname) {
  fprintf(stderr, "Usage: %s [options]\n", progname);
  fprintf(stderr, " -p port     port number (default:%d)\n", PORT);
  fprintf(stderr, " -d num      maximum number of documents (default: no limit)\n");
  fprintf(stderr, " -i size     maximum size of inverted indexes (default:%d)\n",
          static_cast<int>(INV_SIZE));
  fprintf(stderr, " -w nworker  number of worker thread (default:%d)\n",
          static_cast<int>(NUM_WORKER));
  fprintf(stderr, " -f file     load a file (binary format)\n");
  fprintf(stderr, " -h          show help message\n");
  exit(EXIT_FAILURE);
}

/**
 * Parse command-line options.
 * @param argc the number of arguments
 * @param argv arguments
 * @param param output parameters
 */
void parse_options(int argc, char **argv, CommandParam &param) {
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
      param.num_worker = atoi(argv[++i]);
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

/**
 * Create evbuffer. If failed, send 503 response.
 * @param req request
 * @return buffer
 */
evbuffer *create_buffer(evhttp_request *req) {
  evbuffer *buf = evbuffer_new();
  if (!buf) {
    fprintf(stderr, "Failed to create response buffer\n");
    evhttp_send_reply(req, HTTP_SERVUNAVAIL,
                      "Failed to create response buffer", NULL);
    return NULL;
  }
  return buf;
}

/**
 * Write search results to output buffer.
 * @param buf output buffer
 * @param results search results
 *
 * Format:
 * Tokyo \t 10.1    /// document_id \t point
 * Kyoto \t 5.3     /// document_id \t point
 * Osaka \t 2.5     /// document_id \t point
 */
void write_search_result(
  const std::vector<std::pair<std::string, double> > &results, evbuffer *buf) {
  for (size_t i = 0; i < results.size(); i++) {
    evbuffer_add_printf(buf, "%s\t%.3f\n",
                        results[i].first.c_str(), results[i].second);
  }
}

/**
 * Append a job to thread pool.
 * @param req evhttp request object
 * @param arg optional argument
 * @param type request type
 */
void append_job(evhttp_request *req, void *arg, RequestType type) {
  CallbackParam *cbparam = reinterpret_cast<CallbackParam *>(arg);
  WorkerParam *wparam = new WorkerParam(cbparam->handler, cbparam->pool,
                                        req, type);
  cbparam->pool->add(wparam);
}

/**
 * Callback function for 'add' event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_add(evhttp_request *req, void *arg) {
  append_job(req, arg, ADD);
}

/**
 * Callback function for 'delete' event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_delete(evhttp_request *req, void *arg) {
  append_job(req, arg, DELETE);
}

/**
 * Callback function for 'size' event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_size(evhttp_request *req, void *arg) {
  append_job(req, arg, SIZE);
}

/**
 * Callback function for 'dsearch' event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_dsearch(evhttp_request *req, void *arg) {
  append_job(req, arg, DSEARCH);
}

/**
 * Callback function for 'fsearch' event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_fsearch(evhttp_request *req, void *arg) {
  append_job(req, arg, FSEARCH);
}

/**
 * Callback function for 'save' event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_save(evhttp_request *req, void *arg) {
  append_job(req, arg, SAVE);
}

/**
 * Callback function for 'load' event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_load(evhttp_request *req, void *arg) {
  append_job(req, arg, LOAD);
}

/**
 * Callback function of NotFound event.
 * @param req evhttp request object
 * @param arg optional argument
 */
void cb_notfound(evhttp_request *req, void *arg) {
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evhttp_send_reply(req, HTTP_NOTFOUND, "Not found", NULL);
}

/**
 * 'add' handler (search by documents)
 * @param wparam parameter for workers
 */
void add_handler(WorkerParam *wparam) {
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(wparam->req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *id = evhttp_find_header(&headers, "id");
  const char *fstr = evhttp_find_header(&headers, "feature");
  if (id && fstr) {
    std::vector<std::string> features;
    stupa::split_string(fstr, "\t", features);
    wparam->handler->add_document(id, features);
    evhttp_send_reply(wparam->req, HTTP_OK, "OK", NULL);
  } else {
    evhttp_send_reply(wparam->req, HTTP_BADREQUEST,
                      "document id or features not specified", NULL);
  }
  evhttp_clear_headers(&headers);
}

/**
 * 'delete' handler
 * @param wparam parameter for workers
 */
void delete_handler(WorkerParam *wparam) {
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(wparam->req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *id = evhttp_find_header(&headers, "id");
  if (id) {
    wparam->handler->delete_document(id);
    evhttp_send_reply(wparam->req, HTTP_OK, "OK", NULL);
  } else {
    evhttp_send_reply(wparam->req, HTTP_BADREQUEST,
                      "document id not specified", NULL);
  }
  evhttp_clear_headers(&headers);
}

/**
 * 'size' handler
 * @param wparam parameter for workers
 */
void size_handler(WorkerParam *wparam) {
  evbuffer *buf = create_buffer(wparam->req);
  if (buf) {
    evbuffer_add_printf(buf, "%ld\n", wparam->handler->size());
    evhttp_send_reply(wparam->req, HTTP_OK, "OK", buf);
  } else {
    evhttp_send_reply(wparam->req, HTTP_SERVUNAVAIL,
                      "cannot create a response buffer", NULL);
  }
  evbuffer_free(buf); 
}

/**
 * 'dsearch' handler (search by documents)
 * @param wparam parameter for workers
 */
void dsearch_handler(WorkerParam *wparam) {
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(wparam->req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);

  size_t max = MAX_RESULT;
  const char *maxstr = evhttp_find_header(&headers, "max");
  if (maxstr) max = atoi(maxstr);

  const char *query = evhttp_find_header(&headers, "query");
  if (query) {
    evbuffer *buf = create_buffer(wparam->req);
    std::vector<std::string> document_ids;
    stupa::split_string(query, "\t", document_ids);
    std::vector<std::pair<std::string, double> > results;
    wparam->handler->search_by_document(document_ids, results, max);
    write_search_result(results, buf);
    evhttp_send_reply(wparam->req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
  } else {
    evhttp_send_reply(wparam->req, HTTP_BADREQUEST,
                      "document id or features not specified", NULL);
  }
  evhttp_clear_headers(&headers);
}

/**
 * 'fsearch' handler (search by features)
 * @param wparam parameter for workers
 */
void fsearch_handler(WorkerParam *wparam) {
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(wparam->req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);

  size_t max = MAX_RESULT;
  const char *maxstr = evhttp_find_header(&headers, "max");
  if (maxstr) max = atoi(maxstr);

  const char *query = evhttp_find_header(&headers, "query");
  if (query) {
    evbuffer *buf = create_buffer(wparam->req);
    std::vector<std::string> feature_ids;
    stupa::split_string(query, "\t", feature_ids);
    std::vector<std::pair<std::string, double> > results;
    wparam->handler->search_by_feature(feature_ids, results, max);
    write_search_result(results, buf);
    evhttp_send_reply(wparam->req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
  } else {
    evhttp_send_reply(wparam->req, HTTP_BADREQUEST,
                      "feature id or features not specified", NULL);
  }
  evhttp_clear_headers(&headers);
}

/**
 * 'save' handler
 * @param wparam parameter for workers
 */
void save_handler(WorkerParam *wparam) {
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(wparam->req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *filename = evhttp_find_header(&headers, "file");
  if (!filename) {
    evhttp_send_reply(wparam->req, HTTP_BADREQUEST,
                      "filename not specified", NULL);
  }
  if (!wparam->handler->save(filename)) {
    evhttp_send_reply(wparam->req, HTTP_SERVUNAVAIL,
                      "cannot save data to the specified path", NULL);
  }
  evhttp_send_reply(wparam->req, HTTP_OK, "OK", NULL);
  evhttp_clear_headers(&headers);
}

/**
 * 'load' handler
 * @param wparam parameter for workers
 */
void load_handler(WorkerParam *wparam) {
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(wparam->req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *filename = evhttp_find_header(&headers, "file");
  if (!filename) {
    evhttp_send_reply(wparam->req, HTTP_BADREQUEST,
                      "filename not specified", NULL);
  }
  if (!wparam->handler->load(filename)) {
    evhttp_send_reply(wparam->req, HTTP_SERVUNAVAIL,
                      "cannot open file", NULL);
  }
  evhttp_send_reply(wparam->req, HTTP_OK, "OK", NULL);
  evhttp_clear_headers(&headers);
}

void *worker_func(void *arg) {
  stupa::ThreadPool *pool = reinterpret_cast<stupa::ThreadPool *>(arg);
  for (;;) {
    pthread_mutex_lock(pool->queue_mutex());
    while (pool->queue()->empty() && !pool->is_shutdown()) {
      pthread_cond_wait(pool->queue_cond(), pool->queue_mutex());
    }
    if (pool->is_shutdown()) {
      pthread_mutex_unlock(pool->queue_mutex());
      pthread_exit(NULL);
    }
    WorkerParam *wparam =
      reinterpret_cast<WorkerParam *>(pool->queue()->front());
    pool->queue()->pop();
    pthread_mutex_unlock(pool->queue_mutex());
    evhttp_add_header(wparam->req->output_headers, "Content-Type",
                      "text/plain; charset=UTF-8");
    switch (wparam->type) {
      case ADD:
        add_handler(wparam);
        break;
      case DELETE:
        delete_handler(wparam);
        break;
      case SIZE:
        size_handler(wparam);
        break;
      case DSEARCH:
        dsearch_handler(wparam);
        break;
      case FSEARCH:
        fsearch_handler(wparam);
        break;
      case SAVE:
        save_handler(wparam);
        break;
      case LOAD:
        load_handler(wparam);
        break;
      default:
        evhttp_send_reply(wparam->req, HTTP_NOTFOUND, "Not found", NULL);
        break;
    };
    //evhttp_request_free(wparam->req);
    delete wparam;
  }
}

/**
 * Start stupa search server.
 * @param port port number
 */
void start_server(const CommandParam &param) {
  signal(SIGPIPE, SIG_IGN);  // ignore sigpipe
  event_init();
  evhttp *httpd = evhttp_start("0.0.0.0", param.port);
  if (!httpd) {
    fprintf(stderr, "cannot start stupa server\n");
    exit(EXIT_FAILURE);
  }
  stupa::StupaSearchHandler handler(param.invsize, param.max_doc);
  if (param.filename) {
    printf("Load: %s\n", param.filename);
    handler.load(param.filename);
  }
  stupa::ThreadPool pool(param.num_worker, 100);
  pool.init(worker_func);
  CallbackParam cbparam(&handler, &pool);

  // set event handlers
  evhttp_set_cb(httpd, "/add",     cb_add,     &cbparam);
  evhttp_set_cb(httpd, "/delete",  cb_delete,  &cbparam);
  evhttp_set_cb(httpd, "/size",    cb_size,    &cbparam);
  evhttp_set_cb(httpd, "/fsearch", cb_fsearch, &cbparam);
  evhttp_set_cb(httpd, "/dsearch", cb_dsearch, &cbparam);
  evhttp_set_cb(httpd, "/save",    cb_save,    &cbparam);
  evhttp_set_cb(httpd, "/load",    cb_load,    &cbparam);
  evhttp_set_gencb(httpd, cb_notfound, NULL);

  printf("Start stupa search server\n");
  event_dispatch();
  evhttp_free(httpd);
}
