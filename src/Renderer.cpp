
#include "SoftwareRenderer/Renderer.h"

#include <cmath>

namespace sr {

	// Private
	
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

	void Renderer::renderTriangleWireframe(const std::shared_ptr<pw::PixelWindow>& fb, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
		const int width = fb->getWidth();
		const int height = fb->getHeight();
		const auto pos1 = this->transformViewport(v1, width, height).getPosition();
		const auto pos2 = this->transformViewport(v2, width, height).getPosition();
		const auto pos3 = this->transformViewport(v3, width, height).getPosition();
		
		this->renderLine(fb, pos1.getX(), pos1.getY(), pos2.getX(), pos2.getY(), 0xFFFFFFFF);
		this->renderLine(fb, pos2.getX(), pos2.getY(), pos3.getX(), pos3.getY(), 0xFFFFFFFF);
		this->renderLine(fb, pos3.getX(), pos3.getY(), pos1.getX(), pos1.getY(), 0xFFFFFFFF);
	}

	Vertex Renderer::transformViewport(const Vertex& vert, int viewportWidth, int viewportHeight) const {
		Vertex out{};

		const auto& pos = vert.getPosition();
		const auto scale = 1.0f / pos.getW();

		out.position[0] = (pos.getX() * scale + 1.0f) * viewportWidth/2;
		out.position[1] = (pos.getY() * scale + 1.0f) * viewportHeight/2;
		out.position[2] = pos.getZ() * scale;
		out.position[3] = pos.getW() * scale;

		out.color = vert.color;

		return out;
	}

	// Public

	void Renderer::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->frameBuffer = window;
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

	void Renderer::renderTriangleWireframe(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;
		this->renderTriangleWireframe(fb, v1, v2, v3);
	}


	void Renderer::render(RenderMode mode, const std::vector<Vertex>& vertices) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;

		if (mode == RenderMode::TRIANGLE) {
			// TODO Render Triangle (implemented later)
		}
		else if (mode == RenderMode::TRIANGLE_WIREFRAME) {
			// Render Wireframe
			for (size_t i = 0; i < vertices.size(); i += 3) {
				this->renderTriangleWireframe(fb, vertices[i], vertices[i + 1], vertices[i + 2]);
			}
		}
	}

	void Renderer::renderIndexed(RenderMode mode, const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;

		if (mode == RenderMode::TRIANGLE) {
			// TODO Render Triangle (implemented later)
		}
		else if (mode == RenderMode::TRIANGLE_WIREFRAME) {
			// Render Wireframe
			for (size_t i = 0; i < vertices.size(); ++i) {
				auto triangleIndices = indices.getVertexAttribute(i);
				this->renderTriangleWireframe(fb, vertices[triangleIndices[0]], vertices[triangleIndices[1]], vertices[triangleIndices[2]]);
			}
		}
	}

}
