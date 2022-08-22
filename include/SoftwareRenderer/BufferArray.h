#pragma once

#include <array>

namespace sr {

	class BufferArray {
	private:
		std::array<int, 16> buffers;
	public:
		BufferArray() = default;

		void storeInAttributeList(int index, int bufferID);
	};

}
