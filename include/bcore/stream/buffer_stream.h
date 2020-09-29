#ifndef BCORE_BCORE_BUFFER_STREAM_H_
#define BCORE_BCORE_BUFFER_STREAM_H_
#include <streambuf>

namespace bcore {
	class BufferStreamBase;
	class ODataSerialize {
	private:
		static const uint64_t EncodeVarintCompare = 0x7F;
	public:
		inline void Output(bool x, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsStateValid()) {
				buf.sputc((char)(x ? 1 : 0));
			}
		}
		inline void Output(char x, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsStateValid()) {
				buf.sputc(x);
			}
		}
		inline void Output(unsigned char x, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsStateValid()) {
				buf.sputc((char)x);
			}
		}
		inline void Output(uint64_t x, std::streambuf& buf, BufferStreamBase& bs) {
			if (!bs.IsStateValid())
				return;
			while (true) {
				if (x > EncodeVarintCompare) {
					if (-1 == buf.sputc((char)(x & 0x7F | 0x80))) {
						return;
					}
					x >>= 7;
				}
				else {
					if (-1 == buf.sputc((char)(x & 0x7F | 0x80))) {
						return;
					}
				}
			}
		}
		inline void Output(const char* str, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsStateValid()) {
				buf.sputn(str, strlen(str));
			}
		}
		inline void Output(const std::string& str, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsStateValid()) {
				buf.sputn(str.c_str(), str.size());
			}
		}
	};
	class IDataSerialize {
	public:
		void Input(bool& val, std::streambuf& buf, BufferStreamBase& bs) {
			char c = 0;
			if (buf.sgetc(&c) == std::EOF) {
				val = c == 0 ? true : false;
			}
		}
	};
	
	class BufferStreamBase {
	public:
		void SetState(int state) {
			mask_ |= state;
		}
		int IsStateValid() {
			return mask_ != 0;
		}
	private:
		int mask_ = 0;
	};
	//template <class OutputSerialize>
	class IBufferStream : virtual public BufferStreamBase,  public ODataSerialize {
	public:
		IBufferStream& operator>> (bool& val) {
			Output(val);
			return *this;
		}
		IBufferStream& operator>> (short& val) {
			Output(val);
			return *this;
		}
		IBufferStream& operator>> (unsigned short& val) {
			Output(val);
			return *this;
		}
		IBufferStream& operator>> (int& val) {

		}
		IBufferStream& operator>> (unsigned int& val) {

		}
		IBufferStream& operator>> (long& val) {

		}
		IBufferStream& operator>> (unsigned long& val) {

		}
		IBufferStream& operator>> (long long& val) {

		}
		IBufferStream& operator>> (unsigned long long& val) {

		}
		IBufferStream& operator>> (float& val) {

		}
		IBufferStream& operator>> (double& val) {

		}
		IBufferStream& operator>> (long double& val) {

		}
		IBufferStream& operator>> (void*& val) {

		}
	private:
		std::streambuf buf_;
	};
	class OBufferStream : public ODataSerialize {
	public:
		OBufferStream& operator<< (bool val);
		OBufferStream& operator<< (short val);
		OBufferStream& operator<< (unsigned short val);
		OBufferStream& operator<< (int val);
		OBufferStream& operator<< (unsigned int val);
		OBufferStream& operator<< (long val);
		OBufferStream& operator<< (unsigned long val);
		OBufferStream& operator<< (long long val);
		OBufferStream& operator<< (unsigned long long val);
		OBufferStream& operator<< (float val);
		OBufferStream& operator<< (double val);
		OBufferStream& operator<< (long double val);
		OBufferStream& operator<< (void* val);

		OBufferStream& operator<< (streambuf* sb);
		OBufferStream& operator<< (ostream& (*pf)(ostream&));
		OBufferStream& operator<< (ios& (*pf)(ios&));
		OBufferStream& operator<< (ios_base& (*pf)(ios_base&));
	private:
	};
}

#endif // BCORE_BCORE_BUFFER_STREAM_H_
