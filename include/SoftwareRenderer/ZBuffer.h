#pragma once

#include <memory>

namespace sr {

	class ZBuffer {
	private:

		int width = 0;
		int height = 0;

		std::unique_ptr<float[]> buffer;
	public:

		ZBuffer() = default;
		ZBuffer(int width, int height);

		void resize(int width, int height);

		float get(int x, int y) const;
		void set(int x, int y, float value) ;
		void reset();

		int getWidth() const;
		int getHeight() const;
	};

}
