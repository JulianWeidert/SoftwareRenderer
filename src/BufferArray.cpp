#include "SoftwareRenderer/BufferArray.h"

namespace sr {

	void BufferArray::storeInAttributeList(int index, int bufferID) {
		this->buffers[index] = bufferID;
	}

	int BufferArray::getBufferID(int index) const {
		return this->buffers[index];
	}

	void BufferArray::setIndexBuffer(int bufferID) {
		this->indexBuffer = bufferID;
	}

	int BufferArray::getIndexBuffer() const {
		return this->indexBuffer;
	}

	bool BufferArray::hasIndexBuffer() const {
		return this->indexBuffer != -1;
	}

}
