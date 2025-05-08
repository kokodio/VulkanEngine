cmake_minimum_required(VERSION 3.30)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 23)

function(include_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG        ${gitTag}
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
    )

    FetchContent_MakeAvailable(${libName})
endfunction()

include(FetchContent)

find_package(Vulkan REQUIRED)
find_package(SDL3 QUIET)
if (NOT SDL3_FOUND)
    message(STATUS "Getting SDL3 from Github")
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.2.12)
else()
    message(STATUS "Using local SDL3")
endif()

find_package(glm QUIET)
if (NOT glm_FOUND)
    message(STATUS "Getting GLM from Github")
    include_dependency(glm https://github.com/g-truc/glm.git 1.0.1)
else()
    message(STATUS "Using local GLM")
endif()

add_executable(
        VulkanEngine WIN32
        main.cpp
        App.cpp
)

target_include_directories(VulkanEngine PUBLIC ${SDL3_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
target_link_libraries(VulkanEngine PRIVATE SDL3::SDL3 Vulkan::Vulkan glm::glm)


if(WIN32)
    add_custom_command(
            TARGET VulkanEngine POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:VulkanEngine>
            VERBATIM
    )
endif()