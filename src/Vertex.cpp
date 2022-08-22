#include "SoftwareRenderer/Vertex.h"

namespace sr {

	const lm::Vector4f& Vertex::getPosition() const {
		return this->position;
	}

	const lm::Vector4f& Vertex::getColor() const {
		return this->color;
	}

}
