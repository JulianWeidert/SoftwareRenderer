#include "SoftwareRenderer/ZBuffer.h"

#include <cassert>
#include <limits>

#include <iostream>

namespace sr {

	ZBuffer::ZBuffer(int width, int height) : width(width), height(height) {
		this->buffer = std::make_unique<float[]>(size_t(width) * height);
		this->reset();
	}

	void ZBuffer::resize(int width, int height) {
		this->width = width;
		this->height = height;
		this->buffer = std::make_unique<float[]>(size_t(width) * height);
		this->reset();
	}

	float ZBuffer::get(int x, int y) const {
		assert(this->buffer != nullptr);
		assert(!(x < 0 || x >= this->width || y < 0 || y >= this->height));
		return this->buffer[size_t(y) * this->width + x];
	}

	void ZBuffer::set(int x, int y, float value) {
		assert(this->buffer != nullptr);
		assert(!(x < 0 || x >= this->width || y < 0 || y >= this->height));
		this->buffer[size_t(y) * this->width + x] = value;
	}

	void ZBuffer::reset() {
		int pixelCount = this->width * this->height;
		for (int i = 0; i < pixelCount; ++i) this->buffer[i] = std::numeric_limits<float>::infinity();
	}

	int ZBuffer::getWidth() const {
		return this->width;
	}

	int ZBuffer::getHeight() const {
		return this->height;
	}

}

