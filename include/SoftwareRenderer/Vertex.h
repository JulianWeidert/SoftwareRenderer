#pragma once

#include <LeptonMath/Vector.h>

namespace sr {

	class Vertex {
	public:
		lm::Vector4f position;
		lm::Vector4f color;
		lm::Vector3f normal;

		const lm::Vector4f& getPosition() const;
		const lm::Vector4f& getColor() const;
		const lm::Vector3f& getNormal() const;

		friend Vertex operator+(const Vertex& v1, const Vertex& v2);
		friend Vertex operator-(const Vertex& v1, const Vertex& v2);
		friend Vertex operator-(const Vertex& v);
		friend Vertex operator*(float scalar, const Vertex& v);
	};


}
