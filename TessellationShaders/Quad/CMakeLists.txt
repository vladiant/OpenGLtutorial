cmake_minimum_required(VERSION 3.10)

project(QuadTessellationShaders LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

configure_file(fragment.frag . COPYONLY)
configure_file(tessellation.tesc . COPYONLY)
configure_file(tessellation.tese . COPYONLY)
configure_file(vertex.vert . COPYONLY)


# application
add_executable(
    ${PROJECT_NAME}
    util.hpp
    main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${GLM_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        glfw
        GLEW::GLEW
        OpenGL::GL
)
