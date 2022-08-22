#pragma once

#include "DataBuffer.h"

namespace sr {

	class RenderPipeline;

	class BufferManager {
		friend class RenderPipeline;
	private:
		std::vector<FloatDataBuffer<2>> buffer2f;
		std::vector<FloatDataBuffer<3>> buffer3f;
		std::vector<FloatDataBuffer<4>> buffer4f;
		// Add more buffers if necessary
	public:

		BufferManager() = default;

		template<size_t layout>
		const FloatDataBuffer<layout>& getBuffer(int bufferID);
	};

	template<size_t layout>
	const FloatDataBuffer<layout>& BufferManager::getBuffer(int bufferID) {
		return FloatDataBuffer<layout>();
	}
	
	template<>
	inline const FloatDataBuffer<2>& BufferManager::getBuffer<2>(int bufferID) {
		return this->buffer2f[bufferID];
	}

	template<>
	inline const FloatDataBuffer<3>& BufferManager::getBuffer<3>(int bufferID) {
		return this->buffer3f[bufferID];
	}

	template<>
	inline const FloatDataBuffer<4>& BufferManager::getBuffer<4>(int bufferID) {
		return this->buffer4f[bufferID];
	}

}
