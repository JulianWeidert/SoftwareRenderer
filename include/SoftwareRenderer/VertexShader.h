#pragma once

#include <LeptonMath/Vector.h>

namespace sr {

	using vec2 = lm::Vector2f;
	using vec3 = lm::Vector3f;
	using vec4 = lm::Vector4f;

	class VertexShader {
		friend class RenderPipeline;
	protected:

		vec4 out_position;

		virtual void main() = 0;
		
		void reset();

		template<size_t layout>
		lm::Vectorf<layout> getVertexAttribute(int index);
	};


	template<size_t layout>
	lm::Vectorf<layout> VertexShader::getVertexAttribute(int index) {

	}

}
