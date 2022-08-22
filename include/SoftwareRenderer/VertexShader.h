#pragma once

#include <memory>

#include <LeptonMath/Vector.h>

#include "BufferArray.h"
#include "BufferManager.h"

namespace sr {

	using vec2 = lm::Vector2f;
	using vec3 = lm::Vector3f;
	using vec4 = lm::Vector4f;

	class VertexShader {
		friend class RenderPipeline;
	private:
		BufferArray bufferArray;
		std::weak_ptr<BufferManager> bufferManager;

	protected:

		size_t vertexID = 0;

		vec4 out_position;

		virtual void main() = 0;
		
		void reset();

		template<size_t layout>
		lm::Vectorf<layout> getVertexAttribute(int index);
	};

	// TODO Boundchecking
	template<size_t layout>
	lm::Vectorf<layout> VertexShader::getVertexAttribute(int index) {
		auto bm = this->bufferManager.lock();
		if (bm == nullptr) return lm::Vectorf<layout>();
		
		auto bufferID = bufferArray.getBufferID(index);
		const FloatDataBuffer<layout>& floatBuffer = bm->getBuffer<layout>(bufferID);

		
		return floatBuffer.getVertexAttribute(this->vertexID);
	}

}
