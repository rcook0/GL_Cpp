cmake_minimum_required(VERSION 3.18)
project(SoftRaster LANGUAGES CXX)

# ---- Options ----
option(ENABLE_STB "Enable stb_image_write for PNG/JPG export" ON)
option(BUILD_TOOLS "Build ppm2img and ppm_batch tools" ON)
option(BUILD_DEMOS "Build demo scenes" ON)
option(BUILD_TESTS "Build minimal tests" ON)

# ---- Language/flags ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4 /permissive-)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -D_USE_MATH_DEFINES)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable)
endif()

# ---- Include dirs ----
# assume this layout:
#   include/  -> headers (MeshRenderer2D.hpp, RasterBuffer.hpp, Material.hpp, etc.)
#   src/      -> .cpp implementations (your renderer .cpp, tools, demos)
#   include/thirdparty/stb_image_write.h (if stb enabled)
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---- Core library (your engine) ----
# Keep the list explicit to avoid odd surprises.
set(ENGINE_SOURCES
  src/MeshRenderer2D.cpp
  src/Drawing2D.cpp
  src/Projection3D.cpp
  src/View3D.cpp
  src/Transformation3D.cpp
  src/Mesh3D.cpp
  src/MeshBuilders.cpp       # if you split builders into a .cpp
  src/Shading.cpp
  src/Texture2D.cpp          # if non-header-only
)

add_library(soft_raster STATIC ${ENGINE_SOURCES})
target_include_directories(soft_raster PUBLIC ${CMAKE_SOURCE_DIR}/include)

if (ENABLE_STB)
  target_compile_definitions(soft_raster PUBLIC USE_STB_IMAGE_WRITE)
endif()

# Windows needs no extra links; macOS/Linux should be fine with stdc++ only.

# ---- stb one-definition: put IMPLEMENTATION in exactly one TU ----
# Create a tiny cpp that defines STB_IMAGE_WRITE_IMPLEMENTATION if enabled.
if (ENABLE_STB)
  add_library(stb_image_write_obj OBJECT src/stb_image_write_impl.cpp)
  target_include_directories(stb_image_write_obj PUBLIC ${CMAKE_SOURCE_DIR}/include/thirdparty)
  target_compile_definitions(stb_image_write_obj PRIVATE STB_IMAGE_WRITE_IMPLEMENTATION)
  add_dependencies(soft_raster stb_image_write_obj)
  target_sources(soft_raster PRIVATE $<TARGET_OBJECTS:stb_image_write_obj>)
endif()

# ---- Demos ----
if (BUILD_DEMOS)
  add_executable(demo_main src/demo_main.cpp)
  target_link_libraries(demo_main PRIVATE soft_raster)
endif()

# ---- Tools ----
if (BUILD_TOOLS)
  add_executable(ppm2img src/tools/ppm2img.cpp)
  target_link_libraries(ppm2img PRIVATE soft_raster)

  add_executable(ppm_batch src/tools/ppm_batch.cpp)
  target_link_libraries(ppm_batch PRIVATE soft_raster)
endif()

# ---- Tests (very lightweight) ----
if (BUILD_TESTS)
  add_executable(smoke_test src/tests/smoke_test.cpp)
  target_link_libraries(smoke_test PRIVATE soft_raster)
endif()
