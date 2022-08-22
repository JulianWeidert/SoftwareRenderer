
#include <iostream>
#include <vector>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Vector.h>

#include <SoftwareRenderer/RenderPipeline.h>
#include <SoftwareRenderer/VertexShader.h>


class TestVertexShader : public sr::VertexShader {
public:
	void main() {
		out_position = sr::vec4(this->getVertexAttribute<3>(0), 1);
	}
};


int main(){


	auto w1 = std::make_shared<pw::PixelWindow>(640, 480, "Hello SoftwareRenderer");

	std::vector<float> triangle = { 
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,

		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.0f, -1.0f, 0.0f
	};

	sr::RenderPipeline pipeline;
	pipeline.setRenderSurface(w1);
	
	auto vao = pipeline.createBufferArray();
	pipeline.bindBufferArray(vao);

	auto positionBuffer = pipeline.bufferFloatData<3>(triangle);
	pipeline.storeBufferInBufferArray(0, positionBuffer);

	std::shared_ptr<sr::VertexShader> vs = std::make_shared<TestVertexShader>();
	pipeline.bindVertexShader(vs);



	using vec4 = lm::Vector4f;

	while (w1->isActive()) {

		w1->makeCurrent();

		w1->beginFrame();
		w1->setBackgroundColor(0xFFFF0000);

		pipeline.draw(sr::RenderMode::TRIANGLE_WIREFRAME, triangle.size() / 3);

		w1->endFrame();

		w1->pollEvents();
	}
	

	return 0;
}
