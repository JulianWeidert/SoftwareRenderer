
#include <SoftwareRenderer/RenderPipeline.h>

namespace sr {

	RenderPipeline::RenderPipeline() :
		bufferManager(std::make_shared<BufferManager>())
	{}


	int RenderPipeline::createBufferArray() {
		this->bufferArrays.push_back(BufferArray());
		return this->bufferArrays.size() - 1;
	}

	void RenderPipeline::bindBufferArray(int bufferArrayID) {
		this->currentBufferArray = bufferArrayID;
	}

	void RenderPipeline::bindVertexShader(std::weak_ptr<VertexShader> vs) {
		this->vertexShader = vs;
	}

	void RenderPipeline::storeBufferInBufferArray(int index, int bufferID) {
		if (this->currentBufferArray == -1) return;
		this->bufferArrays[currentBufferArray].storeInAttributeList(index, bufferID);
	}

	void RenderPipeline::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->renderer.setRenderSurface(window);
	}

	void RenderPipeline::draw(RenderMode mode, int vertexCount) {
		// Test if VertexShader is present
		auto vs = this->vertexShader.lock();
		if (vs == nullptr && currentBufferArray != -1) return;

		// setze buffer...
		vs->bufferArray = this->bufferArrays[this->currentBufferArray];
		vs->bufferManager = this->bufferManager;
		
		std::vector<Vertex> transformedVertices;
		transformedVertices.reserve(vertexCount);

		// for every vertex do...
		for (size_t i = 0; i < vertexCount; ++i) {
			vs->vertexID = i;
			vs->main();
			transformedVertices.push_back({ vs->out_position });
			vs->reset();
		}

		// Render triangles
		this->renderer.render(mode, transformedVertices);
	}

}
