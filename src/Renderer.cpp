
#include "SoftwareRenderer/Renderer.h"

#include <cmath>

namespace sr {

	void Renderer::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->frameBuffer = window;
	}

	void Renderer::renderLine(const std::shared_ptr<pw::PixelWindow>& fb, int xBegin, int yBegin, int xEnd, int yEnd, int color) {
		int dx = abs(xEnd - xBegin);
		int sx = xBegin < xEnd ? 1 : -1;
		int dy = -std::abs(yEnd - yBegin);
		int sy = yBegin < yEnd ? 1 : -1;
		int err = dx + dy;
		int e2; /* error value e_xy */

		int x = xBegin;
		int y = yBegin;

		while (1) {
			fb->setPixel(x, y, color);
			if (x == xEnd && y == yEnd) break;
			e2 = 2 * err;
			if (e2 > dy) { 
				err += dy;
				x += sx;
			} 
			if (e2 < dx) {
				err += dx;
				y += sy;
			} 
		}
	}

	void Renderer::renderLine(const Point2D& begin, const Point2D& end, int color) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;
		this->renderLine(fb, begin.getX(), begin.getY(), end.getX(), end.getY(), color);
	}

	void Renderer::renderTriangleWireframe(const Point2D& p1, const Point2D& p2, const Point2D& p3, int color) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;
		this->renderLine(fb, p1.getX(), p1.getY(), p2.getX(), p2.getY(), color);
		this->renderLine(fb, p2.getX(), p2.getY(), p3.getX(), p3.getY(), color);
		this->renderLine(fb, p3.getX(), p3.getY(), p1.getX(), p1.getY(), color);
	}

}
