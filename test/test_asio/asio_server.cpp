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
	auto option = std::make_shared<ServerOption>();
	std::atomic_int value;
	auto f = [&](bnet::ISession* ses, void* message) {
		auto msg = static_cast<std::string*>(message);
		//std::cout << "receivexxx:" << *msg << std::endl;
		ses->SendMessage(message);
		delete msg;
		if (value.fetch_add(1) >= 100000) {
			std::cout << value << std::endl;
		}
	};
	option->SetSessionOption({ bnet::SessionOption::SetReceiveMessageFunc(std::move(f)) });
	TcpServer s(pool->AllocContext(10), std::move(option));
	s.StartUp();
	pool->Wait();
	return 0;
}
