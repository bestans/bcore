#ifndef BCORE_BCORE_BUFFER_STREAM_H_
#define BCORE_BCORE_BUFFER_STREAM_H_
#include <streambuf>
#include <stdio.h>

namespace bcore {
	enum class BUFF_STREAM_STATE {
		DECODE_CHAR_FAILED = 0,
		DECODE_UNSIGNED_CHAR_FAILED,
		DECODE_COMMON_NUMBER_FAILED,
		DECODE_STRING_FAILED,
		DECODE_BUFFER_FAILED,
		ENCODE_BOOL_FAILED,
		ENCODE_CHAR_FAILED,
		ENCODE_UNSIGNED_CHAR_FAILED,
		ENCODE_COMMON_NUMBER_FAILED,
		ENCODE_CONST_CHAR_LEN_NOT_ENOUGH,
		ENCODE_STRING_LEN_NOT_ENOUGH,
	};
	class BufferStreamBase {
	public:
		BufferStreamBase(std::_Uninitialized) {}
		BufferStreamBase(std::streambuf* buf) { Init(buf); }
		void SetState(BUFF_STREAM_STATE state) {
			mask_ |= ((int)1 << (int)state);
		}
		~BufferStreamBase() {}
		bool IsStateValid() {
			return mask_ == 0;
		}
		inline bool IsEOF(std::streambuf::int_type c) {
			return std::streambuf::traits_type::eq_int_type(c, std::streambuf::traits_type::eof());
		}
	protected:
		BufferStreamBase() {}
		void Init(std::streambuf* buf) {
			buf_ = buf;
			mask_ = 0;
		}
	protected:
		int mask_ = 0;
		std::streambuf* buf_ = nullptr;
	};
	const char BufEOF = (char)std::streambuf::traits_type::eof();
	class ODataSerialize {
	private:
		static const uint64_t EncodeVarintCompare = 0x7F;
	public:
		inline void Output(bool x, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsEOF(buf.sputc(x))) {
				bs.SetState(BUFF_STREAM_STATE::ENCODE_BOOL_FAILED);
			}
		}
		inline void Output(char x, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsEOF(buf.sputc(x))) {
				bs.SetState(BUFF_STREAM_STATE::ENCODE_CHAR_FAILED);
			}
		}
		inline void Output(unsigned char x, std::streambuf& buf, BufferStreamBase& bs) {
			if (bs.IsEOF(buf.sputc(x))) {
				bs.SetState(BUFF_STREAM_STATE::ENCODE_UNSIGNED_CHAR_FAILED);
			}
		}
		inline void Output(int16_t x, std::streambuf& buf, BufferStreamBase& bs) {
			Output((uint64_t)x & 0xFFFF, buf, bs);
		}
		inline void Output(uint16_t x, std::streambuf& buf, BufferStreamBase& bs) {
			Output((uint64_t)x & 0xFFFF, buf, bs);
		}
		inline void Output(int32_t x, std::streambuf& buf, BufferStreamBase& bs) {
			Output((uint64_t)x & 0xFFFFFFFF, buf, bs);
		}
		inline void Output(uint32_t x, std::streambuf& buf, BufferStreamBase& bs) {
			Output((uint64_t)x & 0xFFFFFFFF, buf, bs);
		}
		inline void Output(int64_t x, std::streambuf& buf, BufferStreamBase& bs) {
			Output((uint64_t)x, buf, bs);
		}
		inline void Output(uint64_t x, std::streambuf& buf, BufferStreamBase& bs) {
			while (true) {
				if (x > EncodeVarintCompare) {
					if (bs.IsEOF(buf.sputc((char)((x & 0x7F) | 0x80)))) {
						bs.SetState(BUFF_STREAM_STATE::ENCODE_COMMON_NUMBER_FAILED);
						return;
					}
					x >>= 7;
				}
				else {
					if (bs.IsEOF(buf.sputc((char)(x & 0x7F)))) {
						bs.SetState(BUFF_STREAM_STATE::ENCODE_COMMON_NUMBER_FAILED);
					}
					return;
				}
			}
		}
		inline void Output(float x, std::streambuf& buf, BufferStreamBase& bs) {
			union { float f; int32_t i; };
			f = x;
			Output((uint64_t)i & 0xFFFFFFFF, buf, bs);
		}
		inline void Output(double x, std::streambuf& buf, BufferStreamBase& bs) {
			union { double f; int64_t i; };
			f = x;
			Output((uint64_t)i, buf, bs);
		}
		inline void Output(const char* str, std::streambuf& buf, BufferStreamBase& bs) {
			auto len = strlen(str);
			Output((uint32_t)len, buf, bs);
			if (!bs.IsStateValid()) {
				return;
			}
			if (buf.sputn(str, len) != len) {
				bs.SetState(BUFF_STREAM_STATE::ENCODE_CONST_CHAR_LEN_NOT_ENOUGH);
			}
		}
		inline void Output(const std::string& str, std::streambuf& buf, BufferStreamBase& bs) {
			if (buf.sputn(str.c_str(), str.size()) != str.size()) {
				bs.SetState(BUFF_STREAM_STATE::ENCODE_STRING_LEN_NOT_ENOUGH);
			}
		}
	};

	class IDataSerialize {
	public:
		void Input(bool& val, std::streambuf& buf, BufferStreamBase& bs) {
			auto v = (char)buf.sbumpc();
			if (v != BufEOF) {
				val = v == 0 ? false : true;
			}
		}
		void Input(char& val, std::streambuf& buf, BufferStreamBase& bs) {
			auto v = buf.sbumpc();
			if (bs.IsEOF(v)) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_CHAR_FAILED);
			}
			else {
				val = (char)v;
			}
		}
		void Input(unsigned char& val, std::streambuf& buf, BufferStreamBase& bs) {
			auto v = buf.sbumpc();
			if (bs.IsEOF(v)) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_UNSIGNED_CHAR_FAILED);
			}
			else {
				val = (unsigned char)v;
			}
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
			uint64_t temp = 0;
			Input(temp, buf, bs);
			val = (uint32_t)(temp & 0xFFFFFFFF);
		}
		void Input(int64_t& val, std::streambuf& buf, BufferStreamBase& bs) {
			uint64_t temp = 0;
			Input(temp, buf, bs);
			val = (int64_t)temp;
		}
		void Input(uint64_t& x, std::streambuf& buf, BufferStreamBase& bs) {
			int shift = 0;
			int i = buf.sbumpc();
			while (!bs.IsEOF(i) && shift <= 63) {
				char c = i;
				auto b = uint8_t(c);
				x |= ((uint64_t)b & 0x7F) << shift;
				shift += 7;
				if (b < (uint8_t)0x80) {
					return;
				}
				i = buf.sbumpc();
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
			uint32_t len = 0;
			Input(len, buf, bs);
			if (!bs.IsStateValid()) {
				return;
			}
			auto src_len = val.size();
			val.resize(src_len + len);
			if (buf.sgetn(const_cast<char*>(val.c_str()) + src_len, len) != len) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_STRING_FAILED);
			}
		}
		void Input(char* str, uint32_t cap, std::streambuf& buf, BufferStreamBase& bs) {
			uint32_t len = 0;
			Input(len, buf, bs);
			if (!bs.IsStateValid()) {
				return;
			}
			if (len > cap) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_BUFFER_FAILED);
				return;
			}
			if (buf.sgetn(str, len) != len) {
				bs.SetState(BUFF_STREAM_STATE::DECODE_BUFFER_FAILED);
			}
		}
	};
	
#define IBUFFER_STREAM_DEFINE(valueType) \
	inline BasicIBufferStream& operator>> (valueType& val) { \
		if (IsStateValid()) { \
			Input(val, *buf_, *this); \
		} \
		return *this; \
	}

	//template <class OutputSerialize>
	template <class DECODE>
	class BasicIBufferStream : virtual public BufferStreamBase,  public DECODE {
	public:
		BasicIBufferStream(std::streambuf* buf) {
			BufferStreamBase::Init(buf);
		}
		IBUFFER_STREAM_DEFINE(bool);
		IBUFFER_STREAM_DEFINE(char);
		IBUFFER_STREAM_DEFINE(unsigned char);
		IBUFFER_STREAM_DEFINE(int16_t);
		IBUFFER_STREAM_DEFINE(uint16_t);
		IBUFFER_STREAM_DEFINE(int32_t);
		IBUFFER_STREAM_DEFINE(uint32_t);
		IBUFFER_STREAM_DEFINE(int64_t);
		IBUFFER_STREAM_DEFINE(uint64_t);
		IBUFFER_STREAM_DEFINE(float);
		IBUFFER_STREAM_DEFINE(double);
		IBUFFER_STREAM_DEFINE(std::string);
	};
#define OBUFFER_STREAM_DEFINE(valueType) \
	inline BasicOBufferStream& operator<< (valueType val) { \
		if (IsStateValid()) { \
			Output(val, *buf_, *this); \
		} \
		return *this; \
	}
	template <class ENCODE>
	class BasicOBufferStream : virtual public BufferStreamBase, public ENCODE {
	public:
		BasicOBufferStream(std::_Uninitialized) : BufferStreamBase(std::_Noinit) {}
		BasicOBufferStream(std::streambuf* buf) : BufferStreamBase(buf) {}
		OBUFFER_STREAM_DEFINE(bool);
		OBUFFER_STREAM_DEFINE(char);
		OBUFFER_STREAM_DEFINE(unsigned char);
		OBUFFER_STREAM_DEFINE(int16_t);
		OBUFFER_STREAM_DEFINE(uint16_t);
		OBUFFER_STREAM_DEFINE(int32_t);
		OBUFFER_STREAM_DEFINE(uint32_t);
		OBUFFER_STREAM_DEFINE(int64_t);
		OBUFFER_STREAM_DEFINE(uint64_t);
		OBUFFER_STREAM_DEFINE(float);
		OBUFFER_STREAM_DEFINE(double);
		OBUFFER_STREAM_DEFINE(const char*);
		inline BasicOBufferStream& operator<< (const std::string& str) {
			if (IsStateValid()) {
				Output(str, *buf_, *this);
			}
			return *this;
		}
	};
	template <class DECODE, class ENCODE>
	class BasicIOBufferStream : public BasicIBufferStream<DECODE>, public BasicOBufferStream<ENCODE> {
	public:
		BasicIOBufferStream(std::streambuf* buf) : BasicIBufferStream<DECODE>(buf), BasicOBufferStream<ENCODE>(std::_Noinit)  { }
	};

	using IBufferStream = BasicIBufferStream<IDataSerialize>;
	using OBufferStream = BasicOBufferStream<ODataSerialize>;
	using IOBufferStream = BasicIOBufferStream<IDataSerialize, ODataSerialize>;
}

#endif // BCORE_BCORE_BUFFER_STREAM_H_
