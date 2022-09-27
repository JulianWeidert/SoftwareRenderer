#include "SoftwareRenderer/Vertex.h"

namespace sr {

	const lm::Vector4f& Vertex::getPosition() const {
		return this->position;
	}

	const lm::Vector4f& Vertex::getColor() const {
		return this->color;
	}

	Vertex operator+(const Vertex& v1, const Vertex& v2) {
		Vertex res;

		res.color = v1.color + v2.color;
		res.position = v1.position + v2.position;

		return res;
	}

	Vertex operator-(const Vertex& v1, const Vertex& v2) {
		Vertex res;

		res.color = v1.color - v2.color;
		res.position = v1.position - v2.position;

		return res;
	}

	 Vertex operator-(const Vertex& v) {
		 Vertex res;

		 res.color = -v.color;
		 res.position = -v.position;

		 return res;
	}

	Vertex operator*(float scalar, const Vertex& v) {
		Vertex res;

		res.color = scalar * v.color;
		res.position = scalar * v.position;

		return res;
	}


}
