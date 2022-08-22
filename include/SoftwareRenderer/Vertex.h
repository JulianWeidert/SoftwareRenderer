#pragma once

#include <LeptonMath/Vector.h>

namespace sr {

	class Vertex {
	public:
		lm::Vector4f position;
		lm::Vector4f color;

		const lm::Vector4f& getPosition() const;
		const lm::Vector4f& getColor() const;
	};


}
