#pragma once

#include <memory>

#include <PixelWindow/PixelWindow.h>

namespace sr {

	class Renderer {
	private:
		std::weak_ptr<pw::PixelWindow> frameBuffer;
		// DepthBuffer for 3D
	public:
		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);
		void renderLine(int beginX, int beginY, int endX, int endY, int color);
	};

}
