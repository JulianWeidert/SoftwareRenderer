#pragma once

#include <vector>
#include <memory>

#include "SoftwareRenderer/Renderer.h"
#include "DataBuffer.h"

namespace sr {

	enum class RenderMode {
		TRIANGLE,
		TRIANGLE_WIREFRAME
	};

	class RenderPipeline {
	private:
		Renderer renderer;

		std::vector<std::unique_ptr<FloatDataBuffer<2>>> buffer2f;
		std::vector<std::unique_ptr<FloatDataBuffer<3>>> buffer3f;
		std::vector<std::unique_ptr<FloatDataBuffer<4>>> buffer4f;

	public:
		
		template<size_t layout>
		int bufferFloatData(const std::vector<float>& data);
		
		
		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);
		
		void draw(RenderMode mode, int vertexCount);
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
