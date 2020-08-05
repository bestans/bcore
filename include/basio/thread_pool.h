#include "asio.hpp"
#include "bcore/data_struct/stat_balance.h"
#include <memory>

namespace bcore_basio {
	class ThreadPoolContext;
	class ThreadPool {
	public:
		ThreadPool(size_t n) : balance_ctx_pool_(n) {
			ctx_list_.reserve(n);
			for (auto& ctx : balance_ctx_pool_.GetContextList()) {
				ctx_list_.push_back(ctx);
				threads_.emplace_back([]() {
					auto guard = asio::make_work_guard(*(ctx_list_[i]));
					ctx_list_[i]->run();
					});
			}
		}
		void Stop() {
			for (auto& ctx : ctx_list_) {
				ctx->stop();
			}
			for (auto& t : threads_) {
				t.join();
			}
		}
		std::shared_ptr<ThreadPoolContext> AllocContext(int weight) {
			auto it = balance_ctx_pool_.AllocContext(weight);
			return std::make_shared<ThreadPoolContext>(it.first, *this, it.second, weight);
		}

	private:
		std::vector<std::shared_ptr<asio::io_context>> ctx_list_;
		std::vector<std::thread> threads_;
		bcore::BalanceContextPool<asio::io_context> balance_ctx_pool_;
	};
	struct ThreadPoolContext {
		ThreadPoolContext(std::shared_ptr<asio::io_context> argCtx, ThreadPool& argPool, int indexArg, int weightArg) :
			ctx(argCtx), pool(argPool), index(indexArg), weight(weightArg) {

		}
		std::shared_ptr<asio::io_context> ctx;
		ThreadPool& pool;
		int index;
		int weight;

		void Free() {
		}
	};
}