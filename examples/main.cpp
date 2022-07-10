
#include <iostream>

#include <SoftwareRenderer/RenderPipeline.h>

#include <PixelWindow/PixelWindow.h>


int main(){


	auto w1 = std::make_shared<pw::PixelWindow>(640, 480, "Hello SoftwareRenderer");

	sr::Renderer renderer;
	renderer.setRenderSurface(w1);

	while (w1->isActive()) {

		w1->makeCurrent();

		w1->beginFrame();
		w1->setBackgroundColor(0xFFFF0000);

		renderer.renderLine(100, 100, 300, 300, 0xFFFF00FF);

		w1->endFrame();

		w1->pollEvents();
	}
	

	return 0;
}
