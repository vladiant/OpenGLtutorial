cmake_minimum_required(VERSION 3.10)

project(6_Textures LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

configure_file(default.frag . COPYONLY)
configure_file(default.vert . COPYONLY)
configure_file(pop_cat.png . COPYONLY)

# stb
add_library(
    StbLib
    stb/include/stb/stb_image.h
    stb/src/stb.cpp
)

set_target_properties(
    StbLib
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        LINKER_LANGUAGE CXX
        SOVERSION 1
        VERSION 1.0.0
)

target_include_directories(
    StbLib
    PUBLIC
        stb/include
)

# application
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
    Texture.h
    Texture.cpp
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
        StbLib
        glfw
        GLEW::GLEW
        OpenGL::GL
)
