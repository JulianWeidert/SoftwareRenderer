
#include <iostream>
#include <vector>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Matrix.h>

#include <SoftwareRenderer/RenderPipeline.h>
#include <SoftwareRenderer/VertexShader.h>


class TestVertexShader : public sr::VertexShader {
private:
	lm::Matrix4x4f projectionMatrix{};
	lm::Matrix4x4f transformationMatrix{};
public:
	TestVertexShader() {
		// Set Identity
		projectionMatrix[0][0] = 1;
		projectionMatrix[1][1] = 1;
		projectionMatrix[2][2] = 1;
		projectionMatrix[3][3] = 1;

		transformationMatrix[0][0] = 1;
		transformationMatrix[1][1] = 1;
		transformationMatrix[2][2] = 1;
		transformationMatrix[3][3] = 1;
	}

	void main() {
		auto in_position = sr::vec4(this->getVertexAttribute<3>(0), 1.0f);
		in_position = transformationMatrix * in_position;
		in_position = in_position - sr::vec4({ 0, 0.5, 2.5, 0 });

		out_position = projectionMatrix * in_position;
		
	}

	void setProjectionMatrix(const lm::Matrix4x4f& mat) {
		this->projectionMatrix = mat;
	}

	void setTransformationMatrix(const lm::Matrix4x4f& mat) {
		this->transformationMatrix = mat;
	}
};

lm::Matrix4x4f createRotationMatrixYAxis(float rad) {
	lm::Matrix4x4f mat{};

	float cos = std::cos(rad);
	float sin = std::sin(rad);

	mat[0][0] = cos;
	mat[0][2] = sin;
	mat[1][1] = 1;
	mat[2][0] = -sin;
	mat[2][2] = cos;
	mat[3][3] = 1;

	return mat;
}


int main(){


	auto w1 = std::make_shared<pw::PixelWindow>(640, 640, "Hello SoftwareRenderer");


	std::vector<float> positions = {
		0.0f,  1.0f,  0.0f,
		0.5f,  0.0f,  0.5f,
	   -0.5f,  0.0f,  0.5f,
	    0.0f,  0.0f, -0.5f,
	};

	std::vector<int> indices = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 1,
		1, 2, 3,
	};

	sr::RenderPipeline pipeline;
	pipeline.setRenderSurface(w1);
	
	auto vao = pipeline.createBufferArray();
	pipeline.bindBufferArray(vao);

	auto positionBuffer = pipeline.bufferFloatData<3>(positions);
	pipeline.storeBufferInBufferArray(0, positionBuffer);

	auto indexBuffer = pipeline.createIndexBuffer(indices);
	pipeline.bindIndexBuffer(indexBuffer);

	std::shared_ptr<TestVertexShader> vs = std::make_shared<TestVertexShader>();
	pipeline.bindVertexShader(vs);

	lm::Matrix4x4f projMat{};
	projMat[0][0] = 1;
	projMat[1][1] = 1;
	projMat[2][2] = 1;
	projMat[3][2] = -1;

	vs->setProjectionMatrix(projMat);
	
	float rad = 0;


	while (w1->isActive()) {
		rad += 0.005f;

		auto transMat = createRotationMatrixYAxis(rad);
		vs->setTransformationMatrix(transMat);

		w1->makeCurrent();

		w1->beginFrame();
		w1->setBackgroundColor(0xFFFF0000);

		pipeline.draw(sr::RenderMode::TRIANGLE_WIREFRAME, positions.size() / 3);

		w1->endFrame();

		w1->pollEvents();
	}
	

	return 0;
}
