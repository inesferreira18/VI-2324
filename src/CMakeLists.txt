cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(VI-RT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g")


# Check for OpenMP
find_package(OpenMP)

if(OPENMP_FOUND)
    message(STATUS "OpenMP found")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    #target_compile_options(main PUBLIC ${OpenMP_CXX_FLAGS})
else()
    message(WARNING "OpenMP not found")
endif()


if  (WIN32)
    # Windows specific configurations if needed
else(WIN32) 
    #Linux and Mac
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(GLEW REQUIRED)
endif(WIN32)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add main executable
add_executable(main main.cpp)
target_link_libraries(main ${SDL2_LIBRARIES})

if (WIN32)
    add_custom_command(
        TARGET main POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:main>"
        VERBATIM
    )
endif()

# Add subdirectories
add_library(
        LibCamera
        Camera/camera.hpp
        Camera/perspective.cpp
        Camera/perspective.hpp
)

add_library(
        LibImage
        Image/image.hpp
        Image/ImagePPM.cpp
        Image/ImagePPM.hpp
)

add_library(LibLight INTERFACE)
target_include_directories(LibLight INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Light)

add_library(
        LibPrimitive
        Primitive/BRDF/BRDF.hpp
        Primitive/BRDF/Phong.cpp
        Primitive/BRDF/Phong.hpp
        Primitive/Geometry/geometry.hpp
        Primitive/Geometry/mesh.hpp
        Primitive/Geometry/mesh.cpp
        Primitive/Geometry/geometry.hpp
        Primitive/Geometry/triangle.hpp
        Primitive/Geometry/triangle.cpp
        Primitive/BB.hpp
        Primitive/primitive.hpp
)

add_library(LibRays INTERFACE)
target_include_directories(LibRays INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Rays)


add_library(
        LibRenderer
        Renderer/renderer.hpp
        Renderer/StandardRenderer.cpp
        Renderer/StandardRenderer.hpp
)

add_library(
        LibScene
        Scene/scene.cpp
        Scene/scene.hpp
        #Scene/loader_example.cc
)

add_library(
        LibShader
        Shader/AmbientShader.cpp
        Shader/WhittedShader.cpp
        Shader/DistributedShader.cpp
        Shader/PathTracerShader.cpp
        Shader/AmbientShader.hpp
        Shader/WhittedShader.hpp
        Shader/DistributedShader.hpp
        Shader/PathTracerShader.hpp
        Shader/shader.hpp
        )


add_library(LibUtils INTERFACE)
target_include_directories(LibUtils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/utils)

# Link libraries
target_link_libraries(main LibCamera) 
target_link_libraries(main LibPrimitive)
target_link_libraries(main LibLight)
target_link_libraries(main LibScene)
target_link_libraries(main LibShader)
target_link_libraries(main LibRays)
target_link_libraries(main LibRenderer)
target_link_libraries(main LibImage)
target_link_libraries(main LibUtils)