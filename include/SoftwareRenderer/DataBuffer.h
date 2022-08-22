#pragma once

#include <vector>
#include <cassert>

#include <LeptonMath/Vector.h>

namespace sr {

	template<typename T, size_t layout>
	class DataBuffer {
	private:
		std::vector<T> data;
	public:
		DataBuffer() = default;
		DataBuffer(const std::vector<T>& data);

		lm::Vector<T, layout> getVertexAttribute(size_t index);

		size_t getAttributeCount() const;
	};

	template<size_t layout>
	using FloatDataBuffer = DataBuffer<float, layout>;

	template<size_t layout>
	using IntegerDataBuffer = DataBuffer<int, layout>;

	// Implementation

	template<typename T, size_t layout>
	DataBuffer<T, layout>::DataBuffer(const std::vector<T>& data) : data(data) {
		assert(data.size() % layout == 0);
		this->data.shrink_to_fit();
	}

	template<typename T, size_t layout>
	lm::Vector<T, layout> DataBuffer<T, layout>::getVertexAttribute(size_t index) {
		size_t begin = index * layout;
		lm::Vector<T, layout> out{};
		for (size_t i = 0; i < layout; ++i) out[i] = this->data[begin + i];
		return out;
	}

	template<typename T, size_t layout>
	size_t DataBuffer<T, layout>::getAttributeCount() const {
		return this->data.size() / layout;
	}

}
