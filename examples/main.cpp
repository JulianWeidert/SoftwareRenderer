
#include <iostream>

#include <PixelWindow/PixelWindow.h>

int main(){


	pw::PixelWindow w = pw::PixelWindow(640, 480, "Hello SoftwareRenderer");

	while (w.isActive()) {

		w.beginFrame();

		w.setBackgroundColor(0xFFFF0000);

		w.endFrame();

		w.pollEvents();
	}
	

	return 0;
}
