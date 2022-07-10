
#include <SoftwareRenderer/RenderPipeline.h>

namespace sr {

	void RenderPipeline::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->renderer.setRenderSurface(window);
	}

}
