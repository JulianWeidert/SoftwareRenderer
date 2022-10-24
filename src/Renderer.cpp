
#include "SoftwareRenderer/Renderer.h"

#include <cmath>

#include <thread>
#include <chrono>

#include <iostream> // DEBUG



namespace sr {

	// RenderBatchContext

	template<size_t size>
	bool Renderer::RenderBatchContext<size>::isFull() const {
		return this->index == size;
	}

	template<size_t size>
	void Renderer::RenderBatchContext<size>::addPixel(int x, int y, int color, float depth) {
		this->buffer[index++] = { x, y, color, depth };
	}

	template<size_t size>
	void Renderer::RenderBatchContext<size>::reset() {
		this->index = 0;
	}

	template<size_t size>
	const std::array<Renderer::Fragment, size>& Renderer::RenderBatchContext<size>::getBuffer() const {
		return this->buffer;
	}

	template<size_t size>
	int Renderer::RenderBatchContext<size>::getIndex() const {
		return this->index;
	}

	// Renderer

	void Renderer::renderPixel(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, int x, int y, int color, float depth) {
		batchContext.addPixel(x, y, color, depth);
		if (batchContext.isFull()) {
			// Write to framebuffer
			auto& pixels = batchContext.getBuffer();
			{
				std::lock_guard<std::mutex> lock(this->frameBufferLock);
				for (auto& p : pixels) {
					if (zBuffer.get(p.x, p.y) > p.depth) {
						fb->setPixel(p.x, p.y, p.color);
						zBuffer.set(p.x, p.y, p.depth);
					}
				}
			}
			batchContext.reset();
		}
	}

	// Wireframe rendering
	void Renderer::renderLine(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, int xBegin, int yBegin, int xEnd, int yEnd, int color) {
		int width = fb->getWidth();
		int height = fb->getHeight();
		
		int dx = abs(xEnd - xBegin);
		int sx = xBegin < xEnd ? 1 : -1;
		int dy = -std::abs(yEnd - yBegin);
		int sy = yBegin < yEnd ? 1 : -1;
		int err = dx + dy;
		int e2; /* error value e_xy */

		int x = xBegin;
		int y = yBegin;

		while (1) {
			if (x < width && x >= 0 && y < height && y >= 0) {
				this->renderPixel(fb, batchContext, x, y, color, 0);
				//fb->setPixel(x, y, color);
			}
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

	void Renderer::renderTriangleWireframe(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
		const int width = fb->getWidth();
		const int height = fb->getHeight();

		const auto pos1 = this->transformViewport(v1, width, height).getPosition();
		const auto pos2 = this->transformViewport(v2, width, height).getPosition();
		const auto pos3 = this->transformViewport(v3, width, height).getPosition();

		this->renderLine(fb, batchContext, pos1.getX(), pos1.getY(), pos2.getX(), pos2.getY(), 0xFFFFFFFF);
		this->renderLine(fb, batchContext, pos2.getX(), pos2.getY(), pos3.getX(), pos3.getY(), 0xFFFFFFFF);
		this->renderLine(fb, batchContext, pos3.getX(), pos3.getY(), pos1.getX(), pos1.getY(), 0xFFFFFFFF);
	}

	void Renderer::renderIndexedTriangleWireframeBatch(const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices, size_t batchBegin, size_t batchSize) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;

		RenderBatchContext<BUFFER_SIZE> renderBatchContext;

		for (size_t i = batchBegin; i < batchBegin + batchSize; ++i) {
			auto triangleIndices = indices.getVertexAttribute(i);

			const auto& v1 = vertices[triangleIndices[0]];
			const auto& v2 = vertices[triangleIndices[1]];
			const auto& v3 = vertices[triangleIndices[2]];

			auto surfaceNormal = this->getSurfaceNormal(v1, v2, v3);

			// Backface culling
			auto pos = lm::Vector3f(v2.getPosition().getXY(), v2.getPosition().getW());
			auto dp = surfaceNormal * -pos;
			if (dp > 0 && backfaceCullingEnabled) continue;

			// Clipping
			auto clipped = this->clipTriangle(v1, v2, v3);
			if (clipped.first == 0) continue;
			const auto& verts = clipped.second;

			this->renderTriangleWireframe(fb, renderBatchContext, verts[0], verts[1], verts[2]);
			if (clipped.first == 2) this->renderTriangleWireframe(fb, renderBatchContext, verts[0], verts[2], verts[3]);
		}

		// Render remaining pixels
		auto& buffer = renderBatchContext.getBuffer();
		for (int i = 0; i < renderBatchContext.getIndex(); ++i) {
			auto& p = buffer[i];
			if (zBuffer.get(p.x, p.y) > p.depth) {
				fb->setPixel(p.x, p.y, p.color);
				zBuffer.set(p.x, p.y, p.color);
			}
		}

	}


	// Triangle rendering

	void Renderer::renderIndexedTriangleBatch(const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices, size_t batchBegin, size_t batchSize) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;
		
		RenderBatchContext<BUFFER_SIZE> renderBatchContext;
		auto geometryShader = this->geometryShader.lock();
		auto fragementShader = this->fragmentShader.lock();
		
		if (geometryShader != nullptr) renderBatchContext.gs =  geometryShader->clone();
		if (fragementShader != nullptr)
			renderBatchContext.fs = fragementShader->clone();
		else
			return; // Fragmentshader is missing
		
		for (size_t i = batchBegin; i < batchBegin + batchSize; ++i) {
			auto triangleIndices = indices.getVertexAttribute(i);

			std::reference_wrapper<const Vertex> v1 = vertices[triangleIndices[0]];
			std::reference_wrapper<const Vertex> v2 = vertices[triangleIndices[1]];
			std::reference_wrapper<const Vertex> v3 = vertices[triangleIndices[2]];

			auto surfaceNormal = this->getSurfaceNormal(v1, v2, v3);

			// Backface culling
			auto pos = lm::Vector3f(v2.get().getPosition().getXY(), v2.get().getPosition().getW());
			auto dp = surfaceNormal * -pos;
			if (dp > 0 && backfaceCullingEnabled) continue;


			auto& gs = renderBatchContext.gs;
			// geometry shader
			if (gs != nullptr) {
				
				gs->in_positions = { v1.get().getPosition(), v2.get().getPosition(), v3.get().getPosition() };
				gs->in_colors = { v1.get().getColor(), v2.get().getColor(), v3.get().getColor() };
				gs->in_surfaceNormal = lm::Vector3f(-surfaceNormal.getXY(), surfaceNormal.getZ());

				gs->main();

				const Vertex out1 = { gs->out_positions[0], gs->out_colors[0] };
				const Vertex out2 = { gs->out_positions[1], gs->out_colors[1] };
				const Vertex out3 = { gs->out_positions[2], gs->out_colors[2] };

				gs->reset();

				v1 = out1;
				v2 = out2;
				v3 = out3;

			}

			auto clipped = this->clipTriangle(v1, v2, v3);
			if (clipped.first == 0) continue;
			const auto& verts = clipped.second;

			this->renderTriangle(fb, renderBatchContext, verts[0], verts[1], verts[2]);
			if (clipped.first == 2) this->renderTriangle(fb, renderBatchContext, verts[0], verts[2], verts[3]);

		}

		// Render remaining pixels
		{
			std::lock_guard<std::mutex> lock(this->frameBufferLock);
			auto& buffer = renderBatchContext.getBuffer();
			for (int i = 0; i < renderBatchContext.getIndex(); ++i) {
				auto& p = buffer[i];
				if (zBuffer.get(p.x, p.y) > p.depth) {
					fb->setPixel(p.x, p.y, p.color);
					zBuffer.set(p.x, p.y, p.depth);
				}
			}
		}

	}

	void Renderer::renderTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
		const int width = fb->getWidth();
		const int height = fb->getHeight();
		
		const auto tv1 = this->transformViewport(v1, width, height);
		const auto tv2 = this->transformViewport(v2, width, height);
		const auto tv3 = this->transformViewport(v3, width, height);

		std::array<std::reference_wrapper<const Vertex>, 3> sorted = this->sortVerticesY(tv1, tv2, tv3);
		
		// generate 4th vertex

		// if dy1 == dy2 => Flat Top
		if(sorted[0].get().getPosition().getY() == sorted[1].get().getPosition().getY()){ // Flat bottom
			if(sorted[0].get().getPosition().getX() < sorted[1].get().getPosition().getX())
				this->renderFlatBottomTriangle(fb, batchContext, sorted[0], sorted[1], sorted[2]);
			else
				this->renderFlatBottomTriangle(fb, batchContext, sorted[1], sorted[0], sorted[2]);
		}
		else if (sorted[1].get().getPosition().getY() == sorted[2].get().getPosition().getY()) { // Flat top
			if (sorted[1].get().getPosition().getX() < sorted[2].get().getPosition().getX())
				this->renderFlatTopTriangle(fb, batchContext, sorted[1], sorted[2], sorted[0]);
			else
				this->renderFlatTopTriangle(fb, batchContext, sorted[2], sorted[1], sorted[0]);
		}
		else {

			float dy1 = sorted[0].get().getPosition().getY() - sorted[1].get().getPosition().getY();
			float dy2 = sorted[0].get().getPosition().getY() - sorted[2].get().getPosition().getY();

			float alpha = dy1 / dy2;
			const auto tv4 = alpha * sorted[2] + (1 - alpha) * sorted[0]; // lerp


			// sorted[1] to v4 is flat


			if (sorted[1].get().getPosition().getX() < tv4.getPosition().getX()) {
				this->renderFlatTopTriangle(fb, batchContext, sorted[1], tv4, sorted[0]);
				this->renderFlatBottomTriangle(fb, batchContext, sorted[1], tv4, sorted[2]);
			}
			else {
				this->renderFlatTopTriangle(fb, batchContext, tv4, sorted[1], sorted[0]);
				this->renderFlatBottomTriangle(fb, batchContext, tv4, sorted[1], sorted[2]);
			}



		}

	}

	void Renderer::renderFlatTopTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& base1, const Vertex& base2, const Vertex& target) {

		float dy = target.getPosition().getY() - base1.getPosition().getY();

		const auto dir1 = 1.0f/dy * (target - base1);
		const auto dir2 = 1.0f/dy * (target - base2);

		int yBegin = std::max(int(std::ceil(base1.getPosition().getY() - 0.5f)), 0);
		int yEnd = std::min(int(std::ceil(target.getPosition().getY() - 0.5f)), fb->getHeight());

		const auto edge1 = base1 + ((float(yBegin) + 0.5f - base1.getPosition().getY()) * dir1);
		const auto edge2 = base2 + ((float(yBegin) + 0.5f - base2.getPosition().getY()) * dir2);

		this->renderFlatTriangle(fb, batchContext, yBegin, yEnd, edge1, edge2, dir1, dir2);

	}

	void Renderer::renderFlatBottomTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& base1, const Vertex& base2, const Vertex& target) {

		float dy = base1.getPosition().getY() - target.getPosition().getY();

		const auto dir1 = 1.0f / dy * (base1 - target);
		const auto dir2 = 1.0f / dy * (base2 - target);

		int yBegin = std::max(int(std::ceil(target.getPosition().getY() - 0.5f)), 0);
		int yEnd = std::min(int(std::ceil(base1.getPosition().getY() - 0.5f)), fb->getHeight());


		const auto edge1 = target + ((float(yBegin) + 0.5f - target.getPosition().getY()) * dir1);
		const auto edge2 = target + ((float(yBegin) + 0.5f - target.getPosition().getY()) * dir2);


		this->renderFlatTriangle(fb, batchContext, yBegin, yEnd, edge1, edge2, dir1, dir2);

	}

	void Renderer::renderFlatTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, int yBegin, int yEnd, Vertex edge1, Vertex edge2, const Vertex& dir1, const Vertex& dir2) {
		for (int y = yBegin; y < yEnd ; ++y) {
			int xBegin = std::max(int(std::ceil(edge1.getPosition().getX() - 0.5f)), 0);
			int xEnd = std::min(int(std::ceil(edge2.getPosition().getX() - 0.5f)), fb->getWidth());
			

			float dx = edge2.getPosition().getX() - edge1.getPosition().getX();
			auto xStep = 1.0f / dx * (edge2 - edge1);

			auto line = edge1 + (float(xBegin) + 0.5f - edge1.getPosition().getX()) * xStep;

			for (int x = xBegin; x < xEnd ; ++x) {
				// z is between -1 and 1

				// Z-Test
				if (this->zBuffer.get(x, y) < line.getPosition().getZ()) continue;

				auto& fs = batchContext.fs;

				fs->in_color = line.getColor();
				fs->in_position = line.getPosition();
				fs->main();

				int color = this->convertColor(fs->out_color);
				
				//fb->setPixel(x, y, color);
				this->renderPixel(fb, batchContext, x, y, color, line.getPosition().getZ());
				

				line = line + xStep;
			}

			edge1 = edge1 + dir1;
			edge2 = edge2 + dir2;
		}

	}


	Vertex Renderer::transformViewport(const Vertex& vert, int viewportWidth, int viewportHeight) const {
		Vertex out{};

		const auto& pos = vert.getPosition();
		const auto scale = 1.0f / pos.getW();

		out.position[0] = (pos.getX() * scale + 1.0f) * viewportWidth/2;
		out.position[1] = (-pos.getY() * scale + 1.0f) * viewportHeight/2;
		out.position[2] = pos.getZ() * scale;
		out.position[3] = pos.getW() * scale;

		out.color = vert.color;

		return out;
	}

	std::array<std::reference_wrapper<const Vertex>, 3> Renderer::sortVerticesY(const Vertex& v1, const Vertex& v2, const Vertex& v3) const {
		std::array<std::reference_wrapper<const Vertex>, 3> sorted = { v1,v2,v3 };

		if (sorted[0].get().getPosition().getY() < sorted[1].get().getPosition().getY()) std::swap(sorted[0], sorted[1]);
		if (sorted[0].get().getPosition().getY() < sorted[2].get().getPosition().getY()) std::swap(sorted[0], sorted[2]);
		if (sorted[1].get().getPosition().getY() < sorted[2].get().getPosition().getY()) std::swap(sorted[1], sorted[2]);

		return sorted;
	}

	Vertex Renderer::lerp(const Vertex& v1, const Vertex& v2, float alpha) const {
		Vertex out{};

		out.position = alpha * v1.getPosition() + (1 - alpha) * v2.getPosition();
		out.color = alpha * v1.getColor() + (1 - alpha) * v2.getColor();

		return out;
	}

	int Renderer::convertColor(const lm::Vector4f& color) const {
		auto c = 255.0f * color;

		int r = int(c.getX()) & 0xFF;
		int g = int(c.getY()) & 0xFF;
		int b = int(c.getZ()) & 0xFF;
		int a = int(c.getW()) & 0xFF;
		return a << 24 | b << 16 | g << 8 | r; // rbga -> abgr
	}

	std::pair<size_t, std::array<Vertex, 4>> Renderer::clipTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) const {
		const bool insideV1 = -v1.getPosition().getW() <= v1.getPosition().getZ();
		const bool insideV2 = -v2.getPosition().getW() <= v2.getPosition().getZ();
		const bool insideV3 = -v3.getPosition().getW() <= v3.getPosition().getZ();

		int inCount = insideV1 + insideV2 + insideV3;


		if(inCount == 3) return std::make_pair<size_t, std::array<Vertex, 4>>(1, {v1, v2, v3});
		if(inCount == 0) return std::make_pair<size_t, std::array<Vertex, 4>>(0, {});

		if (inCount == 1) {
			const Vertex& in = (insideV1 ? v1 : (insideV2 ? v2 : v3));
			const Vertex& out1 = (!insideV1 ? v1 : v2);
			const Vertex& out2 = (!insideV3 ? v3 : v2);
			
			auto near = (in.getPosition().getZ() - in.getPosition().getW()) / 2;

			auto alpha1 = -(in.getPosition().getW() + near) / (out1.getPosition().getW() - in.getPosition().getW());
			auto alpha2 = -(in.getPosition().getW() + near) / (out2.getPosition().getW() - in.getPosition().getW());

			auto v4 = this->lerp(out1, in, alpha1);
			auto v5 = this->lerp(out2, in, alpha2);

			return std::make_pair<size_t, std::array<Vertex, 4>>(1, {in, v4, v5});
		}

		// inCount == 2

		const Vertex& out = (!insideV1 ? v1 : (!insideV2 ? v2 : v3));
		const Vertex& in1 = (insideV1 ? v1 : v2);
		const Vertex& in2 = (insideV3 ? v3 : v2);

		auto near = (out.getPosition().getZ() - out.getPosition().getW()) / 2;

		auto alpha1 = -(in1.getPosition().getW() + near) / (out.getPosition().getW() - in1.getPosition().getW());
		auto alpha2 = -(in2.getPosition().getW() + near) / (out.getPosition().getW() - in2.getPosition().getW());

		auto v4 = this->lerp(out, in1, alpha1);
		auto v5 = this->lerp(out, in2, alpha2);

		return std::make_pair<size_t, std::array<Vertex, 4>>(2, { in1, in2, v5, v4 });
	}

	lm::Vector3f Renderer::getSurfaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) const {
		return lm::cross((v2.getPosition() - v1.getPosition()).getXYZ(), (v3.getPosition() - v1.getPosition()).getXYZ());
	}

	void Renderer::checkZBufferSize() {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;

		if (fb->getWidth() == this->zBuffer.getWidth() && fb->getHeight() == this->zBuffer.getHeight()) return;
		this->zBuffer.resize(fb->getWidth(), fb->getHeight());
	}

	// Public

	void Renderer::setRenderSurface(std::weak_ptr<pw::PixelWindow> window) {
		this->frameBuffer = window;
	}

	void Renderer::enableBackfaceCulling() {
		this->backfaceCullingEnabled = true;
	}

	void Renderer::disableBackfaceCulling() {
		this->backfaceCullingEnabled = false;
	}

	void Renderer::bindFragmentShader(std::weak_ptr<FragmentShader> fs) {
		this->fragmentShader = fs;
	}

	void Renderer::bindGeometryShader(std::weak_ptr<GeometryShader> gs) {
		this->geometryShader = gs;
	}

	void Renderer::renderIndexed(RenderMode mode, const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices) {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;


		if (mode == RenderMode::TRIANGLE) {
			// Insert Multithreading
			//this->renderIndexedTriangleBatch(vertices, indices, 0, indices.getAttributeCount());

			
			int maxBatchSize = 100;
			int batches = indices.getAttributeCount() / maxBatchSize;
			auto mainBatchSize = indices.getAttributeCount() - size_t(batches) * maxBatchSize;

			auto batchRunner = [this, &vertices, &indices](int batchBegin, int batchSize) {
				for (int i = 0; i < 1; ++i) {
					this->renderIndexedTriangleBatch(vertices, indices, batchBegin, batchSize);
				}
			};

			for (int i = 0; i < batches; ++i) {
				this->renderPool.addJob([i, maxBatchSize, &batchRunner] {
					batchRunner(i * maxBatchSize, maxBatchSize);
				});
			}

			batchRunner(batches * maxBatchSize, mainBatchSize); // Run the rest of the work on the main thread
			while (this->renderPool.isBusy());
			

		}
		else if (mode == RenderMode::TRIANGLE_WIREFRAME) {
			int maxBatchSize = 2500;
			int batches = indices.getAttributeCount() / maxBatchSize;
			auto mainBatchSize = indices.getAttributeCount() - size_t(batches) * maxBatchSize;
			
			auto batchRunner = [this, &vertices, &indices](int batchBegin, int batchSize) {
				for (int i = 0; i < 1; ++i) {
					this->renderIndexedTriangleWireframeBatch(vertices, indices, batchBegin, batchSize);
				}
			};

			for (int i = 0; i < batches; ++i) {
				this->renderPool.addJob([i, maxBatchSize, &batchRunner] {
					batchRunner(i * maxBatchSize, maxBatchSize);
				});
			}

			batchRunner(batches * maxBatchSize, mainBatchSize); // Run the rest of the work on the main thread
			while (this->renderPool.isBusy());

		}
	}

	void Renderer::beginFrame() {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;
		fb->beginFrame();
		fb->setBackgroundColor(0x00000000);
		this->checkZBufferSize();
		this->zBuffer.reset();
	}

	void Renderer::endFrame() {
		auto fb = this->frameBuffer.lock();
		if (fb == nullptr) return;
		fb->endFrame();
	}

}
