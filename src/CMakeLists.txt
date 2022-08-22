

set(INCLUDE_DIR "${SoftwareRenderer_SOURCE_DIR}/include/SoftwareRenderer")

set(SRC 
	"${INCLUDE_DIR}/RenderPipeline.h"
	"${INCLUDE_DIR}/Renderer.h"
	"${INCLUDE_DIR}/Vertex.h"
	"RenderPipeline.cpp"
	"Renderer.cpp"
	"Vertex.cpp"
 )

add_library(SoftwareRenderer ${SRC})

# include dir
target_include_directories(SoftwareRenderer PUBLIC "${INCLUDE_DIR}/..")

target_link_libraries(SoftwareRenderer PixelWindow)
target_link_libraries(SoftwareRenderer LeptonMath)