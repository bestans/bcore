#include "basio/tcp_server.h"
#include "basio/tcp_session.h"
#include "bnet/tcp_option.h"
#include "blog/log.h"
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
//
////µ›πÈ÷’÷πÃıº˛
//template <typename T, typename P>
//inline ostream& FormatString(ostream& os, const T& t, const P& p) {
//	return os << "|" << t << "=" << p;
//}
//
////µ›πÈÃÂ
//template <typename T, typename P, typename... Args>
//inline ostream& FormatString(ostream& os, const T& t, const P& p, const Args&... rest) {
//	os << "|" << t << "=" << p;
//	return FormatString(os, rest...);
//}
//
//template <typename... Args>
//ostream& LogString(ostream& os, const char* desc, const Args&... rest) {
//	os << desc;
//	return FormatString(os, rest...);
//}

int main(int argc, char* argv[]) {
	std::cout << "tcp server start\n";
	std::stringstream ss;
	BLOG(LOG_LEVEL::DEBUG, "desc1", "desc2", 11);
	std::cout << std::is_same<std::decay<char[6]>::type, char*>::value << '\n';

	std::cout << ss.str();
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
