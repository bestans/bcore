#ifndef BCORE_BCORE_STREAM_STREAM_H_
#define BCORE_BCORE_STREAM_STREAM_H_
#include "buffer_stream.h"
#include <vector>
namespace bcore {
	template <class ENCODE, class T>
	BasicOBufferStream<ENCODE>& operator<<(BasicOBufferStream<ENCODE>& out, const T& t) {
		t.Encode(out);
		return out;
	}
	template <class DECODE, class T>
	BasicIBufferStream<DECODE>& operator>>(BasicIBufferStream<DECODE>& input, T& t) {
		t.Decode(input);
		return input;
	}

	//vector
	template <class ENCODE, class T, class Alloc, class ContainerT = std::vector<T, Alloc>>
	BasicOBufferStream<ENCODE>& operator<<(BasicOBufferStream<ENCODE>& out, const std::vector<T, Alloc>& vec) {
		out << (uint32_t)vec.size();
		for (auto& it : vec) {
			out << it;
		}
		return out;
	}
	template <class DECODE, class T, class Alloc, class ContainerT = std::vector<T, Alloc>>
	BasicIBufferStream<DECODE>& operator>>(BasicIBufferStream<DECODE>& in, std::vector<T, Alloc>& t) {
		uint32_t size = 0;
		in >> size;
		for (size_t i = 0; i < size; i++) {
			T v;
			in >> v;
			t.emplace_back(std::move(v));
		}
		return in;
	}
}
#endif // !BCORE_BCORE_STREAM_STREAM_H_
