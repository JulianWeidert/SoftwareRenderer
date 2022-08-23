#pragma once

#include <array>

namespace sr {

	class BufferArray {
	private:
		std::array<int, 16> buffers;
		int indexBuffer = -1;
	public:
		BufferArray() = default;

		void storeInAttributeList(int index, int bufferID);
		int getBufferID(int index) const;
		void setIndexBuffer(int bufferID);
		int getIndexBuffer() const;
		bool hasIndexBuffer() const;
	};

}
