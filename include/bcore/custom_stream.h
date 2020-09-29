#ifndef BCORE_BCORE_CUSTOM_STREAM_H_
#define BCORE_BCORE_CUSTOM_STREAM_H_
#include <streambuf>
#include <fstream>
namespace bcore {
	//eback,gptr,egptr => 读缓冲区的头，当前读位置，尾
	//pbase,pptr,epptr => 写缓冲区的头，当前写位置，尾
	//overflow在输出缓冲区不够用时调用
	//underflow和uflow在输入缓冲区无数据时调用,区别是uflow 会让读取位置前进一位，而underflow不会
	//https://blog.csdn.net/turkeyzhou/article/details/8983379
	//https://www.cnblogs.com/wangshaowei/p/11905831.html
	class CustomStream : public std::streambuf {
	public:
		enum { SIZE = 10 };
		CustomStream() {
			memset(buffer, 'j', 10);
			//buffer[3] = ' ';
			setbuf(buffer, SIZE);
		}
		void log() {
			std::cout << hex << gptr() << endl;
		}
	protected:
		int_type overflow(int_type c) {
			if (c != std::EOF) {
				
			}
			return c;
		}
		CustomStream* setbuf(char* s, streamsize n) {
			setp(s, s + n);
			setg(s, s, s);
			return this;
		}
		//输入缓冲区无数据时调用
		std::int_type underflow() override {
			setg(buffer, gptr(), pptr());
			return *gptr();
		}
		//std::int_type uflow() override {
		//	cout << "uflow" << endl;
		//	memset(buffer, 'x', 10);
		//	setg(buffer, buffer, buffer + 10);
		//	return EOF;
		//}
	private:
		char buffer[SIZE];
	};
}
#endif
