#include "basio/tcp_server.h"
#include "basio/tcp_session.h"
#include "bnet/tcp_option.h"
#include "asio.hpp"
#include <memory>
#include <iostream>

using namespace std;
using namespace bnet;
namespace bcore_basio {
	void TcpServer::DoAccept()
	{
		auto conn = std::make_shared<TcpSession>(pool_->AllocContext(1), option_);
		bnet::ErrorCode err;
		if (!conn->StartUp(err)) {
			return;
		}
		acceptor_.async_accept(conn->Socket(),
			[this, conn](asio::error_code ec)
			{
				if (!ec) { conn->Start(); }
				this->DoAccept();
			});
	}
}

using namespace bcore_basio;
int main1(int argc, char* argv[]) {
	//std::cout << "test tcp server\n";

	//asio::io_context io_context;
	//vector<thread> pool;
	//pool.emplace_back([&] {
	//	io_context.run();
	//	});
	//auto guard = asio::make_work_guard(io_context);
	//TcpServer s(io_context, std::atoi(argv[1]));
	//for (auto& t : pool) {
	//	t.join();
	//}
	return 0;
}
int main11(int argc, char* argv[]) {
	std::cout << "test tcp server\n";
	auto pool = std::make_shared<bcore_basio::ThreadPool>(3);
	//TcpServer s(pool->AllocContext(10), std::atoi("1111"));
	pool->Wait();
	return 0;
}