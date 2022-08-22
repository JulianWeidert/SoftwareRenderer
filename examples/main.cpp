
#include <iostream>
#include <vector>
#include <any>

#include <SoftwareRenderer/RenderPipeline.h>
#include <SoftwareRenderer/DataBuffer.h>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Vector.h>

template<size_t layout>
const sr::FloatDataBuffer<layout>& get(int index, const std::vector<std::any>& data) {
	return std::any_cast<sr::FloatDataBuffer<layout>>(data[index]);
}


int main(){


	auto w1 = std::make_shared<pw::PixelWindow>(640, 480, "Hello SoftwareRenderer");

	sr::Renderer renderer;
	renderer.setRenderSurface(w1);

	std::vector<float> triangle = { 
		0, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	sr::FloatDataBuffer<3> buffer { triangle };

	using vec4 = lm::Vector4f;

	while (w1->isActive()) {

		w1->makeCurrent();

		w1->beginFrame();
		w1->setBackgroundColor(0xFFFF0000);

		sr::Vertex v1 { vec4(buffer.getVertexAttribute(0), 1) };
		sr::Vertex v2 { vec4(buffer.getVertexAttribute(1), 1) };
		sr::Vertex v3 { vec4(buffer.getVertexAttribute(2), 1) };

		//renderer.renderTriangleWireframe({ 100,100 }, { 200, 300 }, {300, 100}, 0xFFFF00FF);
		renderer.renderTriangleWireframe(v1, v2, v3);

		w1->endFrame();

		w1->pollEvents();
	}
	

	return 0;
}
