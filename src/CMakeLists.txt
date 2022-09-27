

set(INCLUDE_DIR "${SoftwareRenderer_SOURCE_DIR}/include/SoftwareRenderer")

set(SRC 
	"${INCLUDE_DIR}/RenderPipeline.h"
	"${INCLUDE_DIR}/Renderer.h"
	"${INCLUDE_DIR}/Vertex.h"
	"${INCLUDE_DIR}/DataBuffer.h"
	"${INCLUDE_DIR}/BufferArray.h"
	"${INCLUDE_DIR}/VertexShader.h"
	"${INCLUDE_DIR}/FragmentShader.h"
	"${INCLUDE_DIR}/GeometryShader.h"
	"${INCLUDE_DIR}/ZBuffer.h"
	"${INCLUDE_DIR}/ModelLoader.h"
	"RenderPipeline.cpp"
	"Renderer.cpp"
	"Vertex.cpp"
	"BufferArray.cpp"
	"VertexShader.cpp"
	"FragmentShader.cpp"
	"GeometryShader.cpp"
    "ZBuffer.cpp"
	"ModelLoader.cpp"
)

add_library(SoftwareRenderer ${SRC})

# include dir
target_include_directories(SoftwareRenderer PUBLIC "${INCLUDE_DIR}/..")

target_link_libraries(SoftwareRenderer PixelWindow)
target_link_libraries(SoftwareRenderer LeptonMath)