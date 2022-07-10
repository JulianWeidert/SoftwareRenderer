
#include <SoftwareRenderer/Renderer.h>

namespace sr {

	void Renderer::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->frameBuffer = window;
	}

	void Renderer::renderLine(int xBegin, int yBegin, int xEnd, int yEnd, int color) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;
		
		int dx = xEnd - xBegin;
		int dy = yEnd - yBegin;
		int x = xBegin;
		int y = yBegin;
		int error = dx / 2;

		fb->setPixel(x, y, color);
		while (x < xEnd) {
			x = x + 1;
			error = error - dy;
			if (error < 0) {
				y = y + 1;
				error = error + dx;
			}
			fb->setPixel(x, y, color);
		}
	}

}
