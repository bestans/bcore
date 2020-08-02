#pragma once

namespace asio {
	class io_context;
}
namespace tcp {
	class acceptor;
}
namespace bcore.bnet{
	class TcpServer {
	private:
		asio::io_context& ctx_;
		tcp::acceptor* acceptor_;
	};
}
