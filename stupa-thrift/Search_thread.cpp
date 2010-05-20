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

#include "Search.h"
#include "Search_handler.h"
#include <concurrency/PosixThreadFactory.h>
#include <concurrency/ThreadManager.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TThreadPoolServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

#include "stupa.h"

using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using namespace stupa::thrift;

using boost::shared_ptr;

void start_simple_server(const ServerParam &param);
void start_thread_pool_server(const ServerParam &param);


int main(int argc, char **argv) {
  ServerParam param;
  parse_options(argc, argv, param);
//  start_simple_server(param);
  start_thread_pool_server(param);
  return 0;
}

void start_simple_server(const ServerParam &param) {
  shared_ptr<SearchHandler> handler(
    new SearchHandler(param.invsize, param.max_doc));
  if (param.filename) handler->load(param.filename);
  shared_ptr<TProcessor> processor(new SearchProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(param.port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
}

void start_thread_pool_server(const ServerParam &param) {
  shared_ptr<SearchHandler> handler(
    new SearchHandler(param.invsize, param.max_doc));
  if (param.filename) handler->load(param.filename);
  shared_ptr<TProcessor> processor(new SearchProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(param.port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  shared_ptr<ThreadManager> threadManager =
    ThreadManager::newSimpleThreadManager(param.workerCount);
  shared_ptr<PosixThreadFactory> threadFactory =
    shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
  threadManager->threadFactory(threadFactory);
  threadManager->start();

  TThreadPoolServer server(processor, serverTransport, transportFactory,
                           protocolFactory, threadManager);
  server.serve();
}
