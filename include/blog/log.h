#ifndef BCORE_BLOG_LOG_H_
#define BCORE_BLOG_LOG_H_
#include <stdio.h>
#include <iostream>
#include <sstream>


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
	static void WriteLog()
	static std::stringstream& log()
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
private:
	static std::stringstream& GetStream() {
		static std::stringstream ss;
		return ss;
	}
};
#define BLOG  blog::log()

#endif //BCORE_BLOG_LOG_H_