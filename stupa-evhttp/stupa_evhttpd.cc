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

#include <iostream>
#include <sys/queue.h>
#include <event.h>
#include <evhttp.h>
#include "handler.h"

const int PORT          = 147258;
const size_t INV_SIZE   = 100;
const size_t MAX_RESULT = 50;

/**
 * Parameters to start stupa server
 */
struct Param {
  int    port;         ///< port number.
  size_t max_doc;      ///< maximum number of documents.
  size_t invsize;      ///< maximum size of inverted indexes.
  char   *filename;    ///< path of input file.

  Param() : port(PORT), max_doc(0), invsize(INV_SIZE), filename(NULL) { }
};

/* function prototypes */
void usage(const char *progname);
void parse_options(int argc, char **argv, Param &param);
evbuffer *create_buffer(evhttp_request *req);
void write_search_result(
  const std::vector<std::pair<std::string, double> > &results, evbuffer *buf);
void add_handler(evhttp_request *req, void *arg);
void delete_handler(evhttp_request *req, void *arg);
void size_handler(evhttp_request *req, void *arg);
void dsearch_handler(evhttp_request *req, void *arg);
void fsearch_handler(evhttp_request *req, void *arg);
void save_handler(evhttp_request *req, void *arg);
void load_handler(evhttp_request *req, void *arg);
void generic_handler(evhttp_request *req, void *arg);
void start_server(const Param &param);


/**
 * Main function.
 */
int main(int argc, char **argv) {
  Param param;
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
void parse_options(int argc, char **argv, Param &param) {
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
 * Generic event handler.
 * @param req evhttp request object
 * @param arg optional argument
 */
void generic_handler(evhttp_request *req, void *arg) {
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evhttp_send_reply(req, HTTP_NOTFOUND, "Not found", NULL);
}

/**
 * 'add' handler (search by documents)
 * @param req evhttp request object
 * @param arg optional argument
 */
void add_handler(evhttp_request *req, void *arg) {
  stupa::StupaSearchHandler *handler =
    reinterpret_cast<stupa::StupaSearchHandler *>(arg);
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *id = evhttp_find_header(&headers, "id");
  const char *fstr = evhttp_find_header(&headers, "feature");
  if (id && fstr) {
    std::vector<std::string> features;
    stupa::split_string(fstr, "\t", features);
    handler->add_document(id, features);
    evhttp_send_reply(req, HTTP_OK, "OK", NULL);
  } else {
    evhttp_send_reply(req, HTTP_BADREQUEST,
                      "document id or features not specified", NULL);
  }
}

/**
 * 'delete' handler
 * @param req evhttp request object
 * @param arg optional argument
 */
void delete_handler(evhttp_request *req, void *arg) {
  stupa::StupaSearchHandler *handler =
    reinterpret_cast<stupa::StupaSearchHandler *>(arg);
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *id = evhttp_find_header(&headers, "id");
  if (id) {
    handler->delete_document(id);
    evhttp_send_reply(req, HTTP_OK, "OK", NULL);
  } else {
    evhttp_send_reply(req, HTTP_BADREQUEST, "document id not specified", NULL);
  }
}

/**
 * 'size' handler
 * @param req evhttp request object
 * @param arg optional argument
 */
void size_handler(evhttp_request *req, void *arg) {
  stupa::StupaSearchHandler *handler =
    reinterpret_cast<stupa::StupaSearchHandler *>(arg);
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evbuffer *buf = create_buffer(req);
  if (!buf) return;

  evbuffer_add_printf(buf, "%ld\n", handler->size());
  evhttp_send_reply(req, HTTP_OK, "OK", buf);
  evbuffer_free(buf); 
}

/**
 * 'dsearch' handler (search by documents)
 * @param req evhttp request object
 * @param arg optional argument
 */
void dsearch_handler(evhttp_request *req, void *arg) {
  stupa::StupaSearchHandler *handler =
    reinterpret_cast<stupa::StupaSearchHandler *>(arg);
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);

  size_t max = MAX_RESULT;
  const char *maxstr = evhttp_find_header(&headers, "max");
  if (maxstr) max = atoi(maxstr);

  const char *query = evhttp_find_header(&headers, "query");
  if (query) {
    evbuffer *buf = create_buffer(req);
    std::vector<std::string> document_ids;
    stupa::split_string(query, "\t", document_ids);
    std::vector<std::pair<std::string, double> > results;
    handler->search_by_document(document_ids, results, max);
    write_search_result(results, buf);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
  } else {
    evhttp_send_reply(req, HTTP_BADREQUEST,
                      "document id or features not specified", NULL);
  }
}

/**
 * 'fsearch' handler (search by features)
 * @param req evhttp request object
 * @param arg optional argument
 */
void fsearch_handler(evhttp_request *req, void *arg) {
  stupa::StupaSearchHandler *handler =
    reinterpret_cast<stupa::StupaSearchHandler *>(arg);
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);

  size_t max = MAX_RESULT;
  const char *maxstr = evhttp_find_header(&headers, "max");
  if (maxstr) max = atoi(maxstr);

  const char *query = evhttp_find_header(&headers, "query");
  if (query) {
    evbuffer *buf = create_buffer(req);
    std::vector<std::string> feature_ids;
    stupa::split_string(query, "\t", feature_ids);
    std::vector<std::pair<std::string, double> > results;
    handler->search_by_feature(feature_ids, results, max);
    write_search_result(results, buf);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
  } else {
    evhttp_send_reply(req, HTTP_BADREQUEST,
                      "feature id or features not specified", NULL);
  }
}

/**
 * 'save' handler
 * @param req evhttp request object
 * @param arg optional argument
 */
void save_handler(evhttp_request *req, void *arg) {
  stupa::StupaSearchHandler *handler =
    reinterpret_cast<stupa::StupaSearchHandler *>(arg);
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *filename = evhttp_find_header(&headers, "file");
  if (!filename) {
    evhttp_send_reply(req, HTTP_BADREQUEST, "filename not specified", NULL);
  }
  if (!handler->save(filename)) {
    evhttp_send_reply(req, HTTP_SERVUNAVAIL,
                      "cannot save data to the specified path", NULL);
  }
  evhttp_send_reply(req, HTTP_OK, "OK", NULL);
}

/**
 * 'load' handler
 * @param req evhttp request object
 * @param arg optional argument
 */
void load_handler(evhttp_request *req, void *arg) {
  stupa::StupaSearchHandler *handler =
    reinterpret_cast<stupa::StupaSearchHandler *>(arg);
  evhttp_add_header(req->output_headers, "Content-Type",
                    "text/plain; charset=UTF-8");
  evkeyvalq headers;
  TAILQ_INIT(&headers);
  char *decoded_uri = evhttp_decode_uri(evhttp_request_uri(req));
  evhttp_parse_query(decoded_uri, &headers);
  free(decoded_uri);
  const char *filename = evhttp_find_header(&headers, "file");
  if (!filename) {
    evhttp_send_reply(req, HTTP_BADREQUEST, "filename not specified", NULL);
  }
  if (!handler->load(filename)) {
    evhttp_send_reply(req, HTTP_SERVUNAVAIL,
                      "cannot open file", NULL);
  }
  evhttp_send_reply(req, HTTP_OK, "OK", NULL);
}

/**
 * Start stupa search server.
 * @param port port number
 */
void start_server(const Param &param) {
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
  // set event handlers
  evhttp_set_cb(httpd, "/add",     add_handler,     &handler);
  evhttp_set_cb(httpd, "/delete",  delete_handler,  &handler);
  evhttp_set_cb(httpd, "/size",    size_handler,    &handler);
  evhttp_set_cb(httpd, "/fsearch", fsearch_handler, &handler);
  evhttp_set_cb(httpd, "/dsearch", dsearch_handler, &handler);
  evhttp_set_cb(httpd, "/save",    save_handler,    &handler);
  evhttp_set_cb(httpd, "/load",    load_handler,    &handler);
  evhttp_set_gencb(httpd, generic_handler, NULL);

  printf("Start stupa search server\n");
  event_dispatch();
  evhttp_free(httpd);
}