
#include <iostream>
#include <vector>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Vector.h>

#include <SoftwareRenderer/RenderPipeline.h>
#include <SoftwareRenderer/VertexShader.h>


class TestVertexShader : public sr::VertexShader {
public:
	void main() {
		out_position = { 0,0,0,0 };
	}
};


int main(){


	auto w1 = std::make_shared<pw::PixelWindow>(640, 480, "Hello SoftwareRenderer");

	sr::Renderer renderer;
	renderer.setRenderSurface(w1);

	std::vector<float> triangle = { 
		0, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	sr::RenderPipeline pipeline;
	pipeline.setRenderSurface(w1);
	
	auto vao = pipeline.createBufferArray();
	pipeline.bindBufferArray(vao);

	auto positionBuffer = pipeline.bufferFloatData<3>(triangle);
	pipeline.storeBufferInBufferArray(0, positionBuffer);

	std::shared_ptr<sr::VertexShader> vs = std::make_shared<TestVertexShader>();

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
