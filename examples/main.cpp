
#include <iostream>
#include <vector>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Matrix.h>

#include <SoftwareRenderer/RenderPipeline.h>
#include <SoftwareRenderer/VertexShader.h>
#include <SoftwareRenderer/FragmentShader.h>
#include <SoftwareRenderer/ModelLoader.h>


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
		auto in_position = sr::vec4(2 * this->getVertexAttribute<3>(0), 1.0f);
		in_position = transformationMatrix * in_position;
		in_position = in_position - sr::vec4({ 0, 0.0f, 3.5, 0 });

		out_position = projectionMatrix * in_position;
	}

	void setProjectionMatrix(const lm::Matrix4x4f& mat) {
		this->projectionMatrix = mat;
	}

	void setTransformationMatrix(const lm::Matrix4x4f& mat) {
		this->transformationMatrix = mat;
	}
};

class TestFragmentShader : public sr::FragmentShader {
protected:
	void main() {
		this->out_color = this->in_color;
	}
};


class TestGeometryShader : public sr::GeometryShader {
private:
	sr::vec3 lightPosition = { 100, 100, 100 };

protected:
	void main() {
		

		auto lightDir = lightPosition - this->in_positions[0].getXYZ();

		auto brightness = std::max(0.04f, this->in_surfaceNormal.getNormalized() * lightDir.getNormalized());

		sr::vec4 outColor = { brightness, brightness, brightness, 1.0 };
		//outColor = { 0,1,1,1 };

		this->out_colors = { outColor, outColor, outColor };
		this->out_positions = this->in_positions;
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


lm::Matrix4x4f createProjectionMatrix(float near, int width, int height) {
	lm::Matrix4x4f mat{};

	mat[0][0] = float(height)/float(width);
	mat[1][1] = 1;
	mat[2][2] = -1;
	mat[2][3] = -2 * near;
	mat[3][2] = -1;

	return mat;
}



int main(){

	auto w1 = std::make_shared<pw::PixelWindow>(640, 640, "SoftwareRenderer");

	// Load OBJ Model

	auto model = sr::loadObj("../../../../examples/dragon.obj.txt");
	if (!model.has_value()) return 0;
	auto& [pos, ind] = model.value();

	std::vector<float> positions = {
			// Front
			-0.5f, 0.5f, 0.5f, // Front Top Left
			0.5f, 0.5f, 0.5f, // Front Top Right
			0.5f,-0.5f, 0.5f, // Front Bottom Right
			-0.5f,-0.5f, 0.5f, // Front Bottom Left

			// Back
			-0.5f, 0.5f,-0.5f, // Back Top Left
			0.5f, 0.5f,-0.5f, // Back Top  Right
			0.5f,-0.5f,-0.5f, // Back Bottom Right
			-0.5f,-0.5f,-0.5f // Back Bottom Left
	};

	std::vector<int> indices = {
			//0,1,2, 0,2,3, // Front
			2,1,0, 3,2,0,

			//1,5,6, 1,6,2, // Right
			6,5,1, 2,6,1,

			//5,4,7, 5,7,6, // Back
			7,4,5, 6,7,5,

			//4,0,3, 4,3,7, // Left
			3,0,4, 7,3,4,

			//4,5,1, 4,1,0, // Top
			1,5,4, 0,1,4,

			//3,2,6, 3,6,7 // Bottom
			6,2,3, 7,6,3
	};

	positions = pos;
	indices = ind;

	sr::RenderPipeline pipeline;
	pipeline.setRenderSurface(w1);
	
	auto vao = pipeline.createBufferArray();
	pipeline.bindBufferArray(vao);

	auto positionBuffer = pipeline.bufferFloatData<3>(positions);
	pipeline.storeBufferInBufferArray(0, positionBuffer);

	auto indexBuffer = pipeline.createIndexBuffer(indices);
	pipeline.bindIndexBuffer(indexBuffer);

	auto vs = std::make_shared<TestVertexShader>();
	pipeline.bindVertexShader(vs);

	auto fs = std::make_shared<TestFragmentShader>();
	pipeline.bindFragmentShader(fs);

	auto gs = std::make_shared<TestGeometryShader>();
	pipeline.bindGeometryShader(gs);

	lm::Matrix4x4f projMat = createProjectionMatrix(1, 640, 640);

	vs->setProjectionMatrix(projMat);

	w1->addResizeCallback([&](int w, int h) {
		lm::Matrix4x4f projMat = createProjectionMatrix(1, w, h);
		vs->setProjectionMatrix(projMat);
	});


	float rad = 0;

	//pipeline.disableBackfaceCulling();

	while (w1->isActive()) {
		rad += 0.001f;

		auto transMat = createRotationMatrixYAxis(rad);
		vs->setTransformationMatrix(transMat);

		w1->makeCurrent();

		w1->beginFrame();
		w1->setBackgroundColor(0x00000000);

		pipeline.draw(sr::RenderMode::TRIANGLE, positions.size() / 3);

		w1->endFrame();

		w1->pollEvents();
	}
	

	return 0;
}
