#ifndef BCORE_BCORE_CUSTOM_STREAM_H_
#define BCORE_BCORE_CUSTOM_STREAM_H_
#include <streambuf>
namespace bcore {
	//eback,gptr,egptr => ����������ͷ����ǰ��λ�ã�β
	//pbase,pptr,epptr => д��������ͷ����ǰдλ�ã�β
	//overflow�����������������ʱ����
	//underflow��uflow�����뻺����������ʱ����,������uflow ���ö�ȡλ��ǰ��һλ����underflow����
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
			cout << hex << gptr() << endl;
		}
	protected:
		int_type overflow(int_type c) {
			if (c != std::EOF) {
			}
			this->xsgetn()
			return c;
		}
		streambuf* setbuf(char* s, streamsize n) {
			setp(s, s + n);
			setg(s, s, s);
			return this;
		}
		//���뻺����������ʱ����
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
