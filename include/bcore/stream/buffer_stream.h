#ifndef BCORE_BCORE_BUFFER_STREAM_H_
#define BCORE_BCORE_BUFFER_STREAM_H_

namespace bcore {
	template <class OutputSerialize>
	class OBufferStream : public OutputSerialize {
		OBufferStream& operator>> (bool& val);
		OBufferStream& operator>> (short& val);
		OBufferStream& operator>> (unsigned short& val);
		OBufferStream& operator>> (int& val);
		OBufferStream& operator>> (unsigned int& val);
		OBufferStream& operator>> (long& val);
		OBufferStream& operator>> (unsigned long& val);
		OBufferStream& operator>> (long long& val);
		OBufferStream& operator>> (unsigned long long& val);
		OBufferStream& operator>> (float& val);
		OBufferStream& operator>> (double& val);
		OBufferStream& operator>> (long double& val);
		OBufferStream& operator>> (void*& val);
	private:
		std::StreamBuffer buf_;
	};

	class DataSerialize {
	private:
		static const uint64_t EncodeVarintCompare = 0x7F;
	public:
		inline void Output(bool x, std::streambuf& buf) {
			buf.sputc((char)(x ? 1 : 0));
		}
		inline void Output(char x, std::streambuf& buf) {
			buf.sputc(x);
		}
		inline void Output(unsigned char x, std::streambuf& buf) {
			buf.sputc((char)x);
		}
		inline void Output(uint64_t x, std::streambuf& buf) {
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
		inline void Output(const char* str, std::streambuf& buf) {
			buf.sputn(str, strlen(str));
		}
		inline void Output(const std::string& str, std::streambuf& buf) {
			buf.sputn(str.c_str(), str.size());
		}
	};
}

#endif // BCORE_BCORE_BUFFER_STREAM_H_
