cmake_minimum_required(VERSION 3.28)
project(VulkanEngine)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    endif()
    #    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()
add_subdirectory(volk)

set(CMAKE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.41.0/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=e570f03806b9aae2082ca5b950a9e6b3b41ad56972a78a876aedcaad16653116
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
        NAME SDL3
        GITHUB_REPOSITORY libsdl-org/SDL
        GIT_TAG release-3.2.12
        OPTIONS
        "SDL_RENDER_GPU OFF"
        "SDL_RENDER OFF"
        "SDL_OPENGL OFF"
        "SDL_DIRECTX OFF"
        "SDL_OPENGLES OFF"
        "SDL_GPU OFF"
        "SDL_HAPTIC OFF"
        "SDL_POWER OFF"
        "SDL_SHARED ON"
        "SDL_STATIC OFF"
        "SDL_SENSOR OFF"
        "SDL_CAMERA OFF"
)
CPMAddPackage("gh:g-truc/glm#1.0.1")
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

file(GLOB_RECURSE GLSL_SOURCE_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.comp
)

set(EXECUTABLE_NAME ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        volk
        SDL3::SDL3
        glm
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        VK_NO_PROTOTYPES
        GLM_FORCE_RADIANS
)
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <optional>
        <memory>
        <string>
        <vector>
        <unordered_map>
        <glm/mat4x4.hpp>
        <glm/vec4.hpp>
        <volk.h>
)

foreach (GLSL IN LISTS GLSL_SOURCE_FILES)
    get_filename_component(FILE_NAME "${GLSL}" NAME_WE)
    set(SPIRV "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${FILE_NAME}.spv")

    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND Vulkan::glslc ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

add_custom_target(
        ShadersTarget ALL
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} ShadersTarget)