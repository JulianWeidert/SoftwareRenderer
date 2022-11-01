#pragma once

#include <array>
#include <memory>

#include <LeptonMath/Vector.h>

namespace sr {

	using vec2 = lm::Vector2f;
	using vec3 = lm::Vector3f;
	using vec4 = lm::Vector4f;

	class GeometryShader {
		friend class Renderer;
	protected:

		std::array<vec4, 3> in_positions;
		std::array<vec4, 3> in_colors;
		std::array<vec3, 3> in_normals;
		vec3 in_surfaceNormal;

		std::array<vec4, 3> out_positions;
		std::array<vec4, 3> out_colors;
		std::array<vec3, 3> out_normals;

		virtual void main() = 0;
		virtual std::unique_ptr<GeometryShader> clone() const = 0;
		void reset();

	public:
		virtual ~GeometryShader() = default;
	};

}
