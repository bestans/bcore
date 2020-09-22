#ifndef BCORE_BLOG_LOG_FILE_H_
#define BCORE_BLOG_LOG_FILE_H_
#include <iostream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <memory>
#include <fstream>
#include "blog.h"
#ifndef __linux__
#include "windows.h"
#else
#include "unistd.h"
#endif

namespace bcore {
	class LogFileContext {
	public:
		LogFileContext(const std::string& root_path, const char* log_name) {
			root_path_ = root_path;
			log_name_ = log_name;
		}
		bool init(time_t now) {
			if (next_check_time_ > now) {
				return true;
			}

			std::tm time_struct;
			std::localtime_s(&time_struct, &now);
			std::stringstream ss;
			ss << root_path_ << "/" << std::put_time(&time_struct, "%Y-%m-%d/");
			
			std::filesystem::path p(ss.str());
			std::filesystem::directory_entry dir(p);
			std::error_code err;
			if (!dir.exists(err)) {
				std::filesystem::create_directory(p, err);
				std::filesystem::permissions(p, (std::filesystem::perms)0755, err);
				if (!dir.exists(err)) {
					std::stringstream temp;
					temp << "ERROR:create_directory " << ss.str() << " failed,err=" << err << std::endl;
					std::cout << temp.str();
					return false;
				}
			}
			ss << log_name_;
			if (file_) {
				file_.close();
			}
			file_.open(ss.str(), std::ios_base::app);

			//下次检查时间
			struct tm z;
			memset(&z, 0, sizeof(z));
			z.tm_year = time_struct.tm_year;
			z.tm_mon = time_struct.tm_mon;
			z.tm_mday = time_struct.tm_mday;
			next_check_time_ = mktime(&z);
			return true;
		}
		~LogFileContext() {
			if (file_) {
				file_.close();
			}
		}
		inline void Lock() {
			mutex_.lock();
		}
		inline void Unlock() {
			mutex_.unlock();
		}
		void WriteLog(const std::string& str) {
			Lock();
			if (!init()) {
				Unlock();
				return;
			}
			std::tm time_struct;
			std::localtime_s(&time_struct, &now);
			file_ << std::put_time(&time_struct, "%Y-%m-%d %H:%M:%S|") << str << std::endl;
			file_.flush();
			Unlock();
		}
		
	private:
		std::string root_path_;
		std::string log_name_;
		std::mutex mutex_;
		std::ofstream file_ = nullptr;
		time_t next_check_time_ = 0;
	};
	class LogFileManager : public Singleton<LogFileManager> {
	protected:
		LogFileManager() {
			static const char* g_log_name_list[LOG_COUNT] = {
				"debug.log",
				"trace.log",
				"format.log",
				"warning.log",
				"error.log",
				"crit.log",
			}
			for (int i = 0; i < LOG_COUNT; i++) {
				log_list_[i] = std::make_shared<LogFileContext>(root_path_, g_log_name_list[i])
			}
		}
		bool Init() {
			std::filesystem::path p(root_path_);
			std::error_code err;
			if (!std::filesystem::create_directory(p, err)) {
				std::cout << "create dir root path," + root_path_ << " failed.err=" << err;
				return false;
			}
			std::filesystem::permissions(p, (std::filesystem::perms)0755, err);
			return true;
		}
		static int GetPid() {
#ifndef __linux__
			
			return (int)GetCurrentProcessId();

#else
			return (int)getpid();
#endif
		}
		static void WriteLog(int prior, const std::string& str) {

		}

	private:
		std::string root_path_ = "./";
		std::array<std::shared_ptr<LogFileContext>, LOG_COUNT> log_list_;
	};

}

#endif
