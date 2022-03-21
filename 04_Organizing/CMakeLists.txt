cmake_minimum_required(VERSION 3.10)

project(4_Organizing LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

configure_file(default.frag . COPYONLY)
configure_file(default.vert . COPYONLY)

add_executable(
    ${PROJECT_NAME}
    EBO.h
    EBO.cpp
    VAO.h
    VAO.cpp
    VBO.h
    VBO.cpp
    shaderClass.h
    shaderClass.cpp
    main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        glfw
        GLEW::GLEW
        OpenGL::GL
)
