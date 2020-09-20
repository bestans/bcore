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
		LogFileContext() {
			
		}
		bool Init() {

		}
		~LogFileContext() {
			if (file_) {
				file_.close();
			}
		}
		void WriteLog(const std::string& str) {
			mutex_.lock();
			auto now = time(0);
			if (now >= next_check_time_) {
				file_.close();
			}
			std::tm time_struct;
			std::localtime_s(&time_struct, &now);
			file_ << std::put_time(&time_struct, "%Y-%m-%d %H:%M:%S|") << str << std::endl;
			file_.flush();
			mutex_.unlock();
		}
	private:
		time_t next_check_time_:
		std::mutex mutex_;
		std::ofstream file_;
		std::string root_path_;
	};
	class LogFileManager : public Singleton<LogFileManager> {
	protected:
		LogFileManager() {
			
		}
		bool Init(std::string& err_msg) {

			std::filesystem::path p(root_path_);
			if (!std::filesystem::exists(p)) {
				err_msg += "invalid root path," + root_path_;
				return false;
			}

			std::filesystem::directory_entry dir(p);

			std::error_code err;
			std::filesystem::create_directory(p, err);
			std::filesystem::permissions(p, (std::filesystem::perms)0755, err);
		}
		inline std::string GetDatePath() {
			std::stringstream ss;
			ss << root_path_ << "/" << xx;
			time_t now = time(0);
			std::tm time_struct;
			std::localtime_s(&time_struct, &now);
			ss << std::put_time(&time_struct, "%Y-%m-%d") << "/";
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
		std::string root_path_;

	};

}

#endif
