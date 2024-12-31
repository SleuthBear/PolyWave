cmake_minimum_required(VERSION 3.29)
project(PolyWave)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit")
endif()


include_directories(Dependencies)

# Find CLFW library and include directory
add_library(glfw STATIC IMPORTED
        ../include/glad/glad.h
        ../glad.c
        ../include/STB_IMAGE/stb_image.cpp
        ../include/glm)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/include/lib-arm64/libglfw3.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
add_executable(PolyWave
        include/glad/glad.h
        include/glad/glad.c
        Shader.h
        include/STB_IMAGE/stb_image.cpp
        WaveMesh.cpp
        WaveMesh.h
        main.cpp
)
target_link_libraries(PolyWave glfw)

