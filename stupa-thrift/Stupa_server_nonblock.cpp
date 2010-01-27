//
// Search server implementation using Bayesian Sets
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

#include "Stupa.h"
#include "Stupa_handler.h"
#include <concurrency/PosixThreadFactory.h>
#include <concurrency/ThreadManager.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TNonblockingServer.h>
#include <transport/TServerSocket.h>

#include "stupa.h"

using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using boost::shared_ptr;

const int PORT         = 9090;
const int WORKER_COUNT = 4;
const size_t INV_SIZE  = 100;

void usage(const char *progname);
void start_nonblocking_thread_server(int port, int workerCount, size_t invsize);

int main(int argc, char **argv) {
  int port = PORT;
  int workerCount = WORKER_COUNT;
  size_t invsize = INV_SIZE;

  int i = 1;
  while (i < argc) {
    if (!strcmp(argv[i], "-p")) {
      port = atoi(argv[++i]);
      ++i;
    } else if (!strcmp(argv[i], "-w")) {
      workerCount = atoi(argv[++i]);
      ++i;
    } else if (!strcmp(argv[i], "-i")) {
      invsize = atoi(argv[++i]);
      ++i;
    } else if (!strcmp(argv[i], "-h")) {
      usage(argv[0]);
    } else {
      usage(argv[0]);
    }
  }

  start_nonblocking_thread_server(port, workerCount, invsize);
  return 0;
}

void usage(const char *progname) {
  fprintf(stderr, "Usage: %s [options]\n", progname);
  fprintf(stderr, " -p port     port number (default:%d)\n", PORT);
  fprintf(stderr, " -w nworker  number of worker thread (default:%d)\n",
          WORKER_COUNT);
  fprintf(stderr, " -i size     maximum size of inverted indexes (default:%d)\n",
          static_cast<int>(INV_SIZE));
  fprintf(stderr, " -h          show help message\n");
  exit(1);
}

void start_nonblocking_thread_server(int port, int workerCount, size_t invsize) {
  shared_ptr<StupaHandler> handler(new StupaHandler(invsize));
  shared_ptr<TProcessor> processor(new StupaProcessor(handler));
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  shared_ptr<ThreadManager> threadManager =
    ThreadManager::newSimpleThreadManager(workerCount);
  shared_ptr<PosixThreadFactory> threadFactory =
    shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  threadManager->threadFactory(threadFactory);
  threadManager->start();

  TNonblockingServer server(processor, protocolFactory, port, threadManager);
  server.serve();
}
