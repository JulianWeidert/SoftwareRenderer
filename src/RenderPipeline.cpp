
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

	int RenderPipeline::createIndexBuffer(const std::vector<int>& data) {
		this->indexBufferList.emplace_back(IntegerDataBuffer<3>(data));
		return this->indexBufferList.size() - 1;
	}

	void RenderPipeline::bindIndexBuffer(int bufferID) {
		if (this->currentBufferArray == -1) return;
		this->bufferArrays[this->currentBufferArray].setIndexBuffer(bufferID);
	}


	void RenderPipeline::bindVertexShader(std::weak_ptr<VertexShader> vs) {
		this->vertexShader = vs;
	}

	void RenderPipeline::bindFragmentShader(std::weak_ptr<FragmentShader> fs) {
		this->renderer.bindFragmentShader(fs);
	}

	void RenderPipeline::bindGeometryShader(std::weak_ptr<GeometryShader> gs) {
		this->renderer.bindGeometryShader(gs);
	}

	void RenderPipeline::storeBufferInBufferArray(int index, int bufferID) {
		if (this->currentBufferArray == -1) return;
		this->bufferArrays[this->currentBufferArray].storeInAttributeList(index, bufferID);
	}

	void RenderPipeline::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->renderer.setRenderSurface(window);
	}

	void RenderPipeline::enableBackfaceCulling() {
		this->renderer.enableBackfaceCulling();
	}

	void RenderPipeline::disableBackfaceCulling() {
		this->renderer.disableBackfaceCulling();
	}

	void RenderPipeline::beginFrame() {
		this->renderer.beginFrame();
	}

	void RenderPipeline::endFrame() {
		this->renderer.endFrame();
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
			transformedVertices.push_back({ vs->out_position, vs->out_color, vs->out_normal });
			vs->reset();
		}

		if (this->bufferArrays[this->currentBufferArray].hasIndexBuffer()) {
			this->renderer.renderIndexed(mode, transformedVertices, this->indexBufferList[this->bufferArrays[this->currentBufferArray].getIndexBuffer()]);
		}
		else {
			//this->renderer.render(mode, transformedVertices);
		}

	}

}
