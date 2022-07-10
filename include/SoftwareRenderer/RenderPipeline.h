
#include <vector>
#include <memory>

#include <SoftwareRenderer/Renderer.h>

namespace sr {

	class RenderPipeline {
	private:
		Renderer renderer;
	public:
		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);
	};

}
