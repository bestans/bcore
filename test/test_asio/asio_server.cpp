#include "basio/tcp_server.h"
#include "basio/tcp_session.h"
#include "bnet/tcp_option.h"
#include "asio.hpp"
#include <memory>
#include <iostream>

using namespace std;
using namespace bnet;
using namespace bcore_basio;

int main(int argc, char* argv[]) {
	std::cout << "test tcp server\n";
	auto pool = std::make_shared<bcore_basio::ThreadPool>(3);
	TcpServer s(pool->AllocContext(10), std::atoi("1111"));
	pool->Wait();
	return 0;
}
