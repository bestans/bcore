#ifndef BCORE_BCORE_BUFFER_STREAM_H_
#define BCORE_BCORE_BUFFER_STREAM_H_
#include <streambuf>

namespace bcore {
	class BufferStreamBase;
	enum class BUFF_STREAM_STATE {
		DECODE_COMMON_NUMBER_FAILED = 0;
	};
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
			if (!bs.IsStateValid) {
				return;
			}
			char c = 0;
			if (buf.sgetc(&c) == std::EOF) {
				val = c == 0 ? true : false;
			}
			return 
		}
		void Input(int8_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			if (!bs.IsStateValid) {
				return;
			}
			if (buf.sgetc(&val) == std::EOF) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_COMMON_NUMBER_FAILED)
			}
		}
		void Input(uint8_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			char temp = 0;
			if (buf.sgetc(&val) == std::EOF) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_COMMON_NUMBER_FAILED)
			}
			val = (uint8_t)temp;
		}
		void Input(int16_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			uint64_t temp = 0;
			Input(temp, buf, bs);
			val = (int16_t)(temp & 0xFFFF);
		}
		void Input(uint16_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			uint64_t temp = 0;
			Input(temp, buf, bs);
			val = (uint16_t)(temp & 0xFFFF);
		}
		void Input(int32_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			uint64_t temp = 0;
			Input(temp, buf, bs);
			val = (int32_t)(temp & 0xFFFFFFFF);
		}
		void Input(uint32_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			if (!bs.IsStateValid) {
				return;
			}
			uint64_t temp = 0;
			Input(temp, buf, bs);
			val = (uint32_t)(temp & 0xFFFFFFFF);
		}
		void Input(int64_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			uint64_t temp = 0;
			Input(temp, buf, bs);
			val = (int64_t)temp;
		}
		void Input(uint64_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			if (!bs.IsStateValid) {
				return;
			}
			char c = 0;
			int shift = 0;
			while (buf.sgetc(&c) != std::EOF && shift <= 63) {
				auto b = uint8_t(c);
				x |= ((uint64_t)b & 0x7F) << shift;
				shift += 7;
				if (b < (uint8_t)0x80) {
					return;
				}
			}
			bs.SetState(BUFF_STREAM_STATE::DECODE_COMMON_NUMBER_FAILED);
		}
		void Input(float& val, std::streambuf& buf, BufferStreamBase& bs) {
			uint64_t temp = 0;
			Input(temp, buf, bs);
			union { float f; uint32_t i; };
			i = (uint32_t)(temp & 0xFFFFFFFF);
			val = f;
		}
		void Input(double& val, std::streambuf& buf, BufferStreamBase& bs) {
			union { double f; uint64_t i; };
			Input(i, buf, bs);
			val = f;
		}
		void Input(std::string& val, std::streambuf& buf, BufferStreamBase& bs) {
			if (!bs.IsStateValid) {
				return;
			}
			uint32_t len = 0;
			Input(len, buf, bs);
			if !bs.IsStateValid() {
				return;
			}
			auto src_len = val.size();
			val.resize(src_len + len);
			if (buf.sgetn(val.c_str() + src_len, len) == std::EOF) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_COMMON_NUMBER_FAILED);
			}
		}
	};
	
	class BufferStreamBase {
	public:
		void SetState(BUFF_STREAM_STATE state) {
			mask_ |= ((int)1<<(int)state);
		}
		int IsStateValid() {
			return mask_ != 0;
		}
	private:
		int mask_ = 0;
	};
#define IBUFFER_STREAM_DEFINE(valueType) \
	IBufferStream& operator>> (valueType& val) { \
		Input(val, *buf_, *this); \
		return *this; \
	}

	//template <class OutputSerialize>
	class IBufferStream : virtual public BufferStreamBase,  public IDataSerialize {
	public:
		IBufferStream(std::streambuf* buf) : buf_(buf) {}
		IBUFFER_STREAM_DEFINE(bool);
		IBUFFER_STREAM_DEFINE(int16_t);
		IBUFFER_STREAM_DEFINE(uint16_t);
		IBUFFER_STREAM_DEFINE(int32_t);
		IBUFFER_STREAM_DEFINE(uint32_t);
		IBUFFER_STREAM_DEFINE(int64_t);
		IBUFFER_STREAM_DEFINE(uint64_t);
		IBUFFER_STREAM_DEFINE(long);
		IBUFFER_STREAM_DEFINE(double);
		IBUFFER_STREAM_DEFINE(std::string);
	private:
		std::streambuf* buf_;
	};
#define OBUFFER_STREAM_DEFINE(valueType) \
	OBufferStream& operator>> (valueType val) { \
		Output(val, *buf_, *this); \
		return *this; \
	}
	class OBufferStream : virtual public BufferStreamBase, public ODataSerialize {
	public:
		OBufferStream(std::streambuf* buf) : buf_(buf) {}
		OBUFFER_STREAM_DEFINE(bool);
		OBUFFER_STREAM_DEFINE(int16_t);
		OBUFFER_STREAM_DEFINE(uint16_t);
		OBUFFER_STREAM_DEFINE(int32_t);
		OBUFFER_STREAM_DEFINE(uint32_t);
		OBUFFER_STREAM_DEFINE(int64_t);
		OBUFFER_STREAM_DEFINE(uint64_t);
		OBUFFER_STREAM_DEFINE(long);
		OBUFFER_STREAM_DEFINE(double);
		OBUFFER_STREAM_DEFINE(const char*);
		OBufferStream& operator<< (std::string& str) {
			Output(str, buf_, this);
			return *this;
		}
	private:
		std::streambuf* buf_;
	};
}

#endif // BCORE_BCORE_BUFFER_STREAM_H_
