cmake_minimum_required( VERSION 3.1 )
project(ClosestPointOnMesh)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Prevent in-source builds.
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message (FATAL_ERROR "In-source builds are not permitted; run CMake inside an empty build directory.")
endif ()

# C++ Standards.
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

##############
# Automatically managed dependencies
#
# glm and imgui are git submodules of this project.

set(WEB_ROOT "${CMAKE_SOURCE_DIR}/distant/web")
set(GLM_ROOT "${CMAKE_SOURCE_DIR}/distant/glm")
set(IMGUI_ROOT "${CMAKE_SOURCE_DIR}/distant/imgui")

set(GLM_INC "${GLM_ROOT}")
set(IMGUI_INC "${IMGUI_ROOT}")

if (NOT EXISTS ${GLM_INC} OR
    NOT EXISTS ${IMGUI_INC}/examples)

    message(STATUS "Updating submodules")

    execute_process(COMMAND git submodule update --init --recursive WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Copy imgui glfw implementation file.
# As is, imgui use gl3w and we change it to use GLAD instead.
if (NOT EXISTS ${IMGUI_ROOT}/imgui_impl_glfw_gl3.cpp OR
        NOT EXISTS ${IMGUI_ROOT}/imgui_impl_glfw_gl3.h)

    message(STATUS "Generating imgui glfw implementation files")

    file(COPY
        ${IMGUI_ROOT}/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
        ${IMGUI_ROOT}/examples/opengl3_example/imgui_impl_glfw_gl3.h
        DESTINATION ${IMGUI_ROOT})

    # I don't want imgui to use gl3w, I want it to use GLAD.
    message(STATUS "Replacing gl3w with glad in imgui implementation files")
    FILE(READ ${IMGUI_ROOT}/imgui_impl_glfw_gl3.cpp IMGUI_HEADER_STR)
    string(REPLACE "GL/gl3w.h" "glad/glad.h" IMGUI_HEADER_STR "${IMGUI_HEADER_STR}")
    FILE(WRITE ${IMGUI_ROOT}/imgui_impl_glfw_gl3.cpp "${IMGUI_HEADER_STR}")
endif()
file (GLOB IMGUI_SRC "${IMGUI_ROOT}/*.cpp")

##############
# Non-automatically managed dependencies
#
# This include: assimp, OpenGL and GLFW

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(GLFW REQUIRED)

include_directories(
        ${GLFW_INCLUDE_DIR}
        ${GLM_INC}
        ${OPENGL_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        ${IMGUI_INC}
        ${WEB_INC}
)

##############
# Build glad lib

set (glad_sources
    "${SRC_DIR}/thirdparty/glad/glad.c"
    "${SRC_DIR}/thirdparty/glad/glad.h"
)

add_library(
    glad STATIC
    ${glad_sources}
)

set_target_properties (glad PROPERTIES FOLDER "GLAD")

include_directories("${SRC_DIR}/thirdparty")
include_directories("${SRC_DIR}/thirdparty/glad")

install (TARGETS glad
    DESTINATION lib
)

##############
# Build assets

message(STATUS "Copy resources into ${CMAKE_CURRENT_BINARY_DIR}/resources")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

##############
# Build core lib
set (core_sources
    "${SRC_DIR}/core/closest_point_query.cpp"
    "${SRC_DIR}/core/closest_point_query.h"
    "${SRC_DIR}/core/math.h"
    "${SRC_DIR}/core/mesh.cpp"
    "${SRC_DIR}/core/mesh.h"
    "${SRC_DIR}/core/mesh_point_cloud.cpp"
    "${SRC_DIR}/core/mesh_point_cloud.h"
    "${SRC_DIR}/core/rasterized_mesh.cpp"
    "${SRC_DIR}/core/rasterized_mesh.h"
    "${SRC_DIR}/core/scene.cpp"
    "${SRC_DIR}/core/scene.h"
    "${SRC_DIR}/core/scene_loader.cpp"
    "${SRC_DIR}/core/scene_loader.h"
    "${SRC_DIR}/thirdparty/nanoflann/nanoflann.hpp"
)

add_library(
    core STATIC
    ${core_sources}
)

set_target_properties (core PROPERTIES FOLDER "Core")

include_directories("${SRC_DIR}")
include_directories("${SRC_DIR}/core")
include_directories("${SRC_DIR}/thirdparty")

install (TARGETS core
    DESTINATION lib
)

target_link_libraries(core glad)
target_link_libraries(core ${OPENGL_LIBRARIES})
target_link_libraries(core ${ASSIMP_LIBRARIES})

##############
# Buil app.gui

set (app_gui_sources
    "${SRC_DIR}/gui/orbit_camera.h"
    "${SRC_DIR}/gui/orbit_camera.cpp"
    "${SRC_DIR}/gui/file_system.h"
    "${SRC_DIR}/gui/mainwindow.cpp"
    "${SRC_DIR}/gui/mainwindow.h"
    "${SRC_DIR}/gui/main.cpp"
    "${SRC_DIR}/gui/rasterized_points.cpp"
    "${SRC_DIR}/gui/rasterized_points.h"
    "${SRC_DIR}/gui/string_utils.h"
    "${SRC_DIR}/gui/shader.cpp"
    "${SRC_DIR}/gui/shader.h"
)

include_directories("${SRC_DIR}")
include_directories("${SRC_DIR}/gui")
include_directories("${SRC_DIR}/thirdparty")

add_executable(
    app.gui
    ${app_gui_sources}
    ${GL3W_SRC}
    ${IMGUI_SRC}
    ${WEB_SRC}
)

target_link_libraries(app.gui core)
target_link_libraries(app.gui glad)
target_link_libraries(app.gui ${OPENGL_LIBRARIES})
target_link_libraries(app.gui ${ASSIMP_LIBRARIES})
target_link_libraries(app.gui ${GLFW_glfw_LIBRARY})

if(CMAKE_DL_LIBS)
    target_link_libraries(app.gui ${CMAKE_DL_LIBS})
endif()

target_include_directories(
    app.gui PRIVATE
    ${SRC_DIR}
)
