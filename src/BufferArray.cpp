#include "SoftwareRenderer/BufferArray.h"

namespace sr {

	void BufferArray::storeInAttributeList(int index, int bufferID) {
		this->buffers[index] = bufferID;
	}

}
