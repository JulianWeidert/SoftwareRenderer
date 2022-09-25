#pragma once

#include <memory>
#include <utility>
#include <array>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Vector.h>

#include "Vertex.h"
#include "DataBuffer.h"
#include "FragmentShader.h"

namespace sr {

	typedef lm::Vector<int, 2> Point2D;

	enum class RenderMode {
		TRIANGLE,
		TRIANGLE_WIREFRAME
	};

	class Renderer {
	private:
		std::weak_ptr<pw::PixelWindow> frameBuffer;
		// DepthBuffer for 3D

		std::weak_ptr<FragmentShader> fragmentShader;

		bool backfaceCullingEnabled = true;

		void renderLine(const std::shared_ptr<pw::PixelWindow>& fb, int xBegin, int yBegin, int xEnd, int yEnd, int color);
		void renderTriangleWireframe(const std::shared_ptr<pw::PixelWindow>& fb, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void renderTriangle(const std::shared_ptr<pw::PixelWindow>& fb, const Vertex& v1, const Vertex& v2, const Vertex& v3);

		void renderFlatTopTriangle(const std::shared_ptr<pw::PixelWindow>& fb, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void renderFlatBottomTriangle(const std::shared_ptr<pw::PixelWindow>& fb, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void renderLine(const std::shared_ptr<pw::PixelWindow>& fb, const Vertex& v1, const Vertex& v2, const Vertex& v3, float y, float xBegin, float xEnd);

		Vertex transformViewport(const Vertex& vert, int viewportWidth, int viewportHeight) const;
		std::array<std::reference_wrapper<const Vertex>, 3> sortVerticesY(const Vertex& v1, const Vertex& v2, const Vertex& v3) const;

		Vertex lerp(const Vertex& v1, const Vertex& v2, float alpha) const;
		Vertex interpolateTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, float x, float y) const;
		int convertColor(const lm::Vector4f& color) const;

		std::pair<size_t, std::array<Vertex, 4>> clipTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) const;

		lm::Vector3f getSurfaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) const;

	public:
		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);
		void enableBackfaceCulling();
		void disableBackfaceCulling();

		void bindFragmentShader(std::weak_ptr<FragmentShader> fs);

		void renderLine(const Point2D& begin, const Point2D& end, int color);
		void renderTriangleWireframe(const Point2D& p1, const Point2D& p2, const Point2D& p3, int color);
		void renderTriangleWireframe(const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void renderTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);


		void render(RenderMode mode, const std::vector<Vertex>& vertices);
		void renderIndexed(RenderMode mode, const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices);
	};

}
