#pragma once

#include <vector>
#include <memory>

#include "Renderer.h"
#include "DataBuffer.h"
#include "BufferArray.h"
#include "VertexShader.h"

#include "BufferManager.h"

namespace sr {


	class RenderPipeline {
	private:
		Renderer renderer;

		std::weak_ptr<VertexShader> vertexShader;

		std::shared_ptr<BufferManager> bufferManager;
		std::vector<IntegerDataBuffer<3>> indexBufferList;

		std::vector<BufferArray> bufferArrays;
		int currentBufferArray = -1;

	public:
		
		RenderPipeline();

		int createBufferArray();
		void bindBufferArray(int bufferArrayID);

		int createIndexBuffer(const std::vector<int>& data);
		void bindIndexBuffer(int bufferID);


		void bindVertexShader(std::weak_ptr<VertexShader> vs);

		void storeBufferInBufferArray(int index, int bufferID);

		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);
		void enableBackfaceCulling();
		void disableBackfaceCulling();


		void draw(RenderMode mode, int vertexCount);

		template<size_t layout>
		int bufferFloatData(const std::vector<float>& data);

	};


	template<size_t layout>
	int RenderPipeline::bufferFloatData(const std::vector<float>& data) {
		switch (layout){
		case 2: 
			this->bufferManager->buffer2f.emplace_back(FloatDataBuffer<2>(data));
			return this->bufferManager->buffer2f.size() - 1;
		case 3:
			this->bufferManager->buffer3f.emplace_back(FloatDataBuffer<3>(data));
			return this->bufferManager->buffer3f.size() - 1;
		case 4:
			this->bufferManager->buffer4f.emplace_back(FloatDataBuffer<4>(data));
			return this->bufferManager->buffer4f.size() - 1;
		default:
			assert(false, "Illegal data layout");
			return -1;
			break;
		}
	}


}
