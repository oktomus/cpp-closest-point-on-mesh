cmake_minimum_required( VERSION 3.1 )
project(ClosestPointOnMesh)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set (CMAKE_CXX_STANDARD 11)

##################################################################################
# Externals
##################################################################################
set(GL3W_ROOT "${CMAKE_SOURCE_DIR}/distant/gl3w")
set(GLM_ROOT "${CMAKE_SOURCE_DIR}/distant/glm")
set(IMGUI_ROOT "${CMAKE_SOURCE_DIR}/distant/imgui")
set(WEB_ROOT "${CMAKE_SOURCE_DIR}/distant/web")

set(GL3W_INC "${GL3W_ROOT}/include")
set(GLM_INC "${GLM_ROOT}")
set(IMGUI_INC "${IMGUI_ROOT}")

set(GL3W_SRC "${GL3W_ROOT}/src/gl3w.c")

if (NOT EXISTS ${GL3W_INC} OR
       # NOT EXISTS ${GLFW_INC} OR
        NOT EXISTS ${GLM_INC} OR
        NOT EXISTS ${IMGUI_INC}/examples)
        message(STATUS "Updating submodules")
        execute_process(COMMAND git submodule update --init --recursive WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (NOT EXISTS ${GL3W_SRC})
        message(STATUS "Generating gl3w source files")
        message("python ${GL3W_ROOT}")
        execute_process(COMMAND python gl3w_gen.py
                                        WORKING_DIRECTORY ${GL3W_ROOT} RESULT_VARIABLE GL3W_CODE OUTPUT_VARIABLE GL3W_LOG ERROR_VARIABLE GL3W_LOG)
        message("GL3W code generation output:\n${GL3W_CODE}\n${GL3W_LOG}")    
endif()

if (NOT EXISTS ${IMGUI_ROOT}/imgui_impl_glfw_gl3.cpp OR
        NOT EXISTS ${IMGUI_ROOT}/imgui_impl_glfw_gl3.h)
    message(STATUS "Generating imgui glfw implementation files")
    file(COPY
        ${IMGUI_ROOT}/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
        ${IMGUI_ROOT}/examples/opengl3_example/imgui_impl_glfw_gl3.h
        DESTINATION ${IMGUI_ROOT}) 
endif()

file (GLOB IMGUI_SRC "${IMGUI_ROOT}/*.cpp")

# Compile GLFW
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#add_subdirectory(${GLFW_ROOT})

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
set(GLFW_LOCATION "${CMAKE_SOURCE_DIR}/distant/glfw")
find_package(GLFW REQUIRED)

include_directories(
        ${GLFW_INCLUDE_DIR}
        ${GL3W_INC}
        ${GLM_INC}
        ${OPENGL_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        ${IMGUI_INC} 
        ${WEB_INC}        
        )


##############
# Buil app.gui

set (app_gui_sources
    "${SRC_DIR}/gui/file_system.h"
    "${SRC_DIR}/gui/mainwindow.cpp"
    "${SRC_DIR}/gui/mainwindow.h"
    "${SRC_DIR}/gui/main.cpp"
)

include_directories("${SRC_DIR}/gui")

add_executable(
    app.gui
    ${app_gui_sources}
    ${GL3W_SRC}
    ${IMGUI_SRC}
    ${WEB_SRC}
)

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