#include "asio.hpp"


#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include "asio.hpp"
#include "basio/tcp_client.h"
#include "basio/thread_pool.h"

using asio::ip::tcp;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

enum { max_length = 1024 };

int main11(int argc, char* argv[])
{
	try
	{
		auto ptr = std::make_shared<asio::io_context>();
		std::cout << "xxx\n";
		if (argc != 4)
		{
			std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
			return 1;
		}


		asio::io_context& io_context = *ptr;

		tcp::socket s(io_context);
		tcp::resolver resolver(io_context);
		asio::connect(s, resolver.resolve(argv[1], argv[2]));

		std::cout << "Enter message: ";
		char request[max_length];
		std::cin.getline(request, max_length);
		size_t request_length = std::strlen(request);

		auto times = atoi(argv[3]);
		auto beginTime = high_resolution_clock::now();
		char reply[max_length];
		for (int i = 0; i < times; i++) {
			asio::write(s, asio::buffer(request, request_length));

			size_t reply_length = asio::read(s,
				asio::buffer(reply, request_length));
		}
		auto timeInterval = std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - beginTime);
		std::cout << "cost time " << timeInterval.count() << ",times=" << times << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
using namespace bcore_basio;
int main(int argc, char* argv[]) {
	std::cout << "test tcp client\n";
	auto pool = std::make_shared<bcore_basio::ThreadPool>(3);
	auto option = std::make_shared<bnet::ClientOption>();
	option->SetClientOption({ bnet::ClientOption::SetConnectIp(argv[0]),
		bnet::ClientOption::SetConnectPort(std::atoi(argv[1]))});
	TcpClient client(pool->AllocContext(10), option);
	auto err = client.StartUp();
	if (!err) {
		std::cout << err.message() << std::endl;
	}
	pool->Wait();
	return 0;
}