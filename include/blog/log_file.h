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
	enum LOG_PRIOR {
		LOG_DEBUG,
		LOG_TRACE,
		LOG_FORMAT,
		LOG_WARNING,
		LOG_ERR,
		LOG_CRIT,
		LOG_COUNT,
	};
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
			localtime_s(&time_struct, &now);
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
			auto now = time(0);
			if (!init(now)) {
				Unlock();
				return;
			}
			std::tm time_struct;
			localtime_s(&time_struct, &now);
			file_ << std::put_time(&time_struct, "%Y-%m-%d %H:%M:%S|") << GetPid() << "|" << str << std::endl;
			file_.flush();
			Unlock();
		}
		static int GetPid() {
#ifndef __linux__
			static int g_pid = (int)GetCurrentProcessId();
#else
			static int g_pid = (int)getpid();
#endif
			return g_pid;
		}
		
	private:
		std::string root_path_;
		std::string log_name_;
		std::mutex mutex_;
		std::ofstream file_;
		time_t next_check_time_ = 0;
	};
	class LogFileManager : public Singleton<LogFileManager> {
	public:
		LogFileManager() {
		}
		bool Init(std::string log_root = "./", bool local = false) {
			root_path_ = log_root;
			std::filesystem::path p(root_path_);
			std::error_code err;
			std::filesystem::directory_entry dir(p);
			if (!dir.exists() && !std::filesystem::create_directory(p, err)) {
				std::cout << "create dir root path," + root_path_ << " failed.err=" << err << std::endl;
				return false;
			}
			std::filesystem::permissions(p, (std::filesystem::perms)0755, err);

			static const char* g_log_name_list[LOG_COUNT] = {
				"debug.log",
				"trace.log",
				"format.log",
				"warning.log",
				"error.log",
				"crit.log",
			};
			for (int i = 0; i < LOG_COUNT; i++) {
				log_list_[i] = std::make_shared<LogFileContext>(root_path_, g_log_name_list[i]);
			}
			init_ = true;
			return true;
		}
		static void WriteLog(LOG_PRIOR prior, const std::string& str) {
			if (Instance().init_) {
				Instance().log_list_[prior]->WriteLog(str);
			}
			if (Instance().local_) {
				auto now = time(0);
				std::stringstream ss;
				std::tm time_struct;
				localtime_s(&time_struct, &now);
				ss << std::put_time(&time_struct, "%Y-%m-%d %H:%M:%S|") << str;
				std::cout << ss.str() << std::endl;
			}
		}

	private:
		std::string root_path_ = "./";
		std::array<std::shared_ptr<LogFileContext>, LOG_COUNT> log_list_;
		bool init_ = false;
		bool local_ = true;
	};

}

#endif
