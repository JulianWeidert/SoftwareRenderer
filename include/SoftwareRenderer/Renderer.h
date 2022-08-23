#pragma once

#include <memory>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Vector.h>

#include "Vertex.h"
#include "DataBuffer.h"

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

		void renderLine(const std::shared_ptr<pw::PixelWindow>& fb, int xBegin, int yBegin, int xEnd, int yEnd, int color);
		void renderTriangleWireframe(const std::shared_ptr<pw::PixelWindow>& fb, const Vertex& v1, const Vertex& v2, const Vertex& v3);

		Vertex transformViewport(const Vertex& vert, int viewportWidth, int viewportHeight) const;
	public:
		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);

		void renderLine(const Point2D& begin, const Point2D& end, int color);
		void renderTriangleWireframe(const Point2D& p1, const Point2D& p2, const Point2D& p3, int color);
		void renderTriangleWireframe(const Vertex& v1, const Vertex& v2, const Vertex& v3);


		void render(RenderMode mode, const std::vector<Vertex>& vertices);
		void renderIndexed(RenderMode mode, const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices);
	};

}
