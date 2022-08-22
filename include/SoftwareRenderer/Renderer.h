#pragma once

#include <memory>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Vector.h>

namespace sr {

	typedef lm::Vector<int, 2> Point2D;

	class Renderer {
	private:
		std::weak_ptr<pw::PixelWindow> frameBuffer;
		// DepthBuffer for 3D

		void renderLine(const std::shared_ptr<pw::PixelWindow>& fb, int xBegin, int yBegin, int xEnd, int yEnd, int color);
	public:
		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);

		void renderLine(const Point2D& begin, const Point2D& end, int color);
		void renderTriangleWireframe(const Point2D& p1, const Point2D& p2, const Point2D& p3, int color);
	};

}
