#pragma once

#include <vector>
#include <memory>

#include "Renderer.h"
#include "DataBuffer.h"
#include "BufferArray.h"
#include "VertexShader.h"

namespace sr {


	class RenderPipeline {
	private:
		Renderer renderer;

		std::weak_ptr<VertexShader> vertexShader;

		std::vector<std::unique_ptr<FloatDataBuffer<2>>> buffer2f;
		std::vector<std::unique_ptr<FloatDataBuffer<3>>> buffer3f;
		std::vector<std::unique_ptr<FloatDataBuffer<4>>> buffer4f;

		std::vector<BufferArray> bufferArrays;
		int currentBufferArray = -1;

	public:
		
		int createBufferArray();
		void bindBufferArray(int bufferArrayID);

		void storeBufferInBufferArray(int index, int bufferID);

		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);
		
		void draw(RenderMode mode, int vertexCount);

		template<size_t layout>
		int bufferFloatData(const std::vector<float>& data);
	};


	template<size_t layout>
	int RenderPipeline::bufferFloatData(const std::vector<float>& data) {
		switch (layout){
		case 2: 
			buffer2f.push_back(std::make_unique<FloatDataBuffer<2>>(data));
			return buffer2f.size() - 1;
		case 3:
			buffer3f.push_back(std::make_unique<FloatDataBuffer<3>>(data));
			return buffer3f.size() - 1;
		case 4:
			buffer4f.push_back(std::make_unique<FloatDataBuffer<4>>(data));
			return buffer4f.size() - 1;
		default:
			assert(false, "Illegal data layout");
			return -1;
			break;
		}
	}


}
