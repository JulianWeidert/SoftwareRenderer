
#include <SoftwareRenderer/RenderPipeline.h>

namespace sr {

	int RenderPipeline::createBufferArray() {
		this->bufferArrays.push_back(BufferArray());
		return this->bufferArrays.size() - 1;
	}

	void RenderPipeline::bindBufferArray(int bufferArrayID) {
		this->currentBufferArray = bufferArrayID;
	}

	void RenderPipeline::storeBufferInBufferArray(int index, int bufferID) {
		if (this->currentBufferArray == -1) return;
		this->bufferArrays[currentBufferArray].storeInAttributeList(index, bufferID);
	}

	void RenderPipeline::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->renderer.setRenderSurface(window);
	}

	void RenderPipeline::draw(RenderMode mode, int vertexCount) {
		std::vector<Vertex> transformedVertices;
		transformedVertices.reserve(vertexCount);

		// for every vertex do...

	}

}
