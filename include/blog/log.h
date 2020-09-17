#ifndef BCORE_BLOG_LOG_H_
#define BCORE_BLOG_LOG_H_
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <filesystem>

enum class LOG_LEVEL {
	DEBUG = 0,
	TRACE,
	FORMAT_LEVEL,
	ERROR_LEVEL,
	INFO_LEVEL,
};
class blog {
public:
	//µ›πÈ÷’÷πÃıº˛
	template <typename T, typename P>
	static std::ostream& FormatString(std::ostream& os, const T& t, const P& p) {
		return os << "|" << t << "=" << p;
	}

	//µ›πÈÃÂ
	template <typename T, typename P, typename... Args>
	static std::ostream& FormatString(std::ostream& os, const T& t, const P& p, const Args&... rest) {
		os << "|" << t << "=" << p;
		return FormatString(os, rest...);
	}
	template <typename... Args>
	static void log(LOG_LEVEL log_level, const char* file, int line, const char* desc, const Args&... rest) {
		static char* LogLevelDescList[] = {
			"debug",
			"trace",
		};
		std::stringstream ss;
		ss << LogLevelDescList[int(log_level)] << "|" << file << ":" << line << "|" << desc;
		FormatString(ss, rest...);
		std::cout << ss.str();
	}
};

class LogWriter {
public:
	LogWriter(const char* file, int line) : file_(file), line_(line), ss_(GetStream()) {
		
	}
	~LogWriter() {

	}
	static std::stringstream& GetStream() {
		static thread_local std::stringstream ss;
		ss.clear();
		return ss;
	}

	template <typename P>
	std::ostream& FormatString(std::ostream& os, const char* t, const P& p) {
		return os << "|" << t << "=" << p << "\n";
	}
	//µ›πÈ÷’÷πÃıº˛
	template <typename T, typename P>
	std::ostream& FormatString(std::ostream& os, const T& t, const P& p) {
		return os << "|" << t << "=" << p << "\n";
	}

	//µ›πÈÃÂ
	template <typename T, typename P, typename... Args>
	std::ostream& FormatString(std::ostream& os, const T& t, const P& p, const Args&... rest) {
		os << "|" << t << "=" << p;
		return FormatString(os, rest...);
	}
	template <typename... Args>
	void log(LOG_LEVEL log_level, const char* desc, const Args&... rest) {
		static char* LogLevelDescList[] = {
			"debug",
			"trace",
		};
		std::stringstream ss;
		ss << LogLevelDescList[int(log_level)] << "|" << file_ << ":" << line_ << "|" << desc;
		FormatString(ss, rest...);
		std::cout << ss.str();
	}
	void init() {
		std::filesystem::path p;
		std::filesystem::directory_entry dir(p);
		std::error_code err;
		std::filesystem::create_directory(p, err);
		std::filesystem::permissions(p, (std::filesystem::perms)0755, err);
	}
private:
	std::stringstream& ss_;
	const char* file_;
	int line_;
};
class LogManager {
	void Init() {
		std::string root;

	}
};
#define BLOG  LogWriter(__FILE__, __LINE__).log

#endif //BCORE_BLOG_LOG_H_