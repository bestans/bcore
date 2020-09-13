#include "basio/tcp_server.h"
#include "basio/tcp_session.h"
#include "bnet/tcp_option.h"
#include "asio.hpp"
#include <memory>
#include <iostream>

using namespace std;
using namespace bnet;
using namespace bcore_basio;
class Base {

};
class Devide : public Base {

};

int main(int argc, char* argv[]) {
	std::cout << "tcp server start\n";
	auto pool = std::make_shared<bcore_basio::ThreadPool>(10);
	auto option = std::make_shared<ServerOption>();
	std::atomic_int value;
	option->SetReceiveMessageFunc<std::string>([&](bnet::ISession* ses, std::string* msg) {
		//std::cout << "receivexxx:" << *msg << std::endl;
		ses->SendMessage(msg);
		delete msg;

		if (++value % 100000 == 0) {
			std::cout << value << std::endl;
		}}
	);
	TcpServer s(pool->AllocContext(10), std::move(option));
	s.StartUp();
	pool->Wait();
	return 0;
}
