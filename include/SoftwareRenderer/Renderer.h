#pragma once

#include <memory>
#include <utility>
#include <array>
#include <mutex>

#include <PixelWindow/PixelWindow.h>
#include <LeptonMath/Vector.h>

#include "Vertex.h"
#include "DataBuffer.h"
#include "FragmentShader.h"
#include "GeometryShader.h"
#include "ZBuffer.h"
#include "ThreadPool.h"


namespace sr {

	#define BUFFER_SIZE 1024

	typedef lm::Vector<int, 2> Point2D;

	enum class RenderMode {
		TRIANGLE,
		TRIANGLE_WIREFRAME
	};

	class Renderer {
	private:

		class Fragment {
		public:
			int x;
			int y;
			int color;
			float depth;
		};

		template<size_t size>
		class RenderBatchContext {
			friend Renderer;
		private:
			std::unique_ptr<FragmentShader> fs;
			std::unique_ptr<GeometryShader> gs;

			std::array<Fragment, size> buffer = { 0 };
			int index = 0;
		public:
			bool isFull() const;
			void addPixel(int x, int y, int color, float depth);
			void reset();
			const std::array<Renderer::Fragment, size>& getBuffer() const;
			int getIndex() const;
		};

		ThreadPool<16> renderPool;
		//  FPS Full HD Dragon model
		//  1 Thread : 21 FPS
		//  2 Threads: 35 FPS
		//  4 Threads: 50 FPS
		//  8 Threads: 55 FPS
		// 16 Threads: 56 FPS

		std::weak_ptr<pw::PixelWindow> frameBuffer;
		ZBuffer zBuffer;

		std::mutex frameBufferLock;

		std::weak_ptr<FragmentShader> fragmentShader;
		std::weak_ptr<GeometryShader> geometryShader;

		bool backfaceCullingEnabled = true;

		void renderPixel(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, int x, int y, int color, float depth);

		// For wireframe rendering
		void renderLine(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, int xBegin, int yBegin, int xEnd, int yEnd, int color);
		void renderTriangleWireframe(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void renderIndexedTriangleWireframeBatch(const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices, size_t batchBegin, size_t batchSize);

		// For triangle rendering
		void renderIndexedTriangleBatch(const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices, size_t batchBegin, size_t batchSize);
		void renderTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void renderFlatTopTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& base1, const Vertex& base2, const Vertex& target);
		void renderFlatBottomTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, const Vertex& base1, const Vertex& base2, const Vertex& target);
		void renderFlatTriangle(const std::shared_ptr<pw::PixelWindow>& fb, RenderBatchContext<BUFFER_SIZE>& batchContext, int yBegin, int yEnd, Vertex edge1, Vertex edge2, const Vertex& dir1, const Vertex& dir2);

		Vertex transformViewport(const Vertex& vert, int viewportWidth, int viewportHeight) const;
		std::array<std::reference_wrapper<const Vertex>, 3> sortVerticesY(const Vertex& v1, const Vertex& v2, const Vertex& v3) const;

		Vertex lerp(const Vertex& v1, const Vertex& v2, float alpha) const;
		int convertColor(const lm::Vector4f& color) const;

		std::pair<size_t, std::array<Vertex, 4>> clipTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) const;

		lm::Vector3f getSurfaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) const;

		void checkZBufferSize();

	public:
		void setRenderSurface(std::weak_ptr<pw::PixelWindow> window);
		void enableBackfaceCulling();
		void disableBackfaceCulling();

		void bindFragmentShader(std::weak_ptr<FragmentShader> fs);
		void bindGeometryShader(std::weak_ptr<GeometryShader> gs);

		void beginFrame();
		void endFrame();

		void render(RenderMode mode, const std::vector<Vertex>& vertices);
		void renderIndexed(RenderMode mode, const std::vector<Vertex>& vertices, const IntegerDataBuffer<3>& indices);
	};

}
