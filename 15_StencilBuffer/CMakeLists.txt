cmake_minimum_required(VERSION 3.10)

project(15_StencilBuffer LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

configure_file(default.frag . COPYONLY)
configure_file(default.vert . COPYONLY)
configure_file(outlining.frag . COPYONLY)
configure_file(outlining.vert . COPYONLY)
# Model data
configure_file(./crow/diffuse.png ./crow COPYONLY)
configure_file(./crow/scene.bin ./crow COPYONLY)
configure_file(./crow/scene.gltf ./crow COPYONLY)
configure_file(./crow-outline/diffuse.png ./crow-outline COPYONLY)
configure_file(./crow-outline/scene.bin ./crow-outline COPYONLY)
configure_file(./crow-outline/scene.gltf ./crow-outline COPYONLY)


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

# json
add_library(
    JsonLib
    json/json.hpp
)

set_target_properties(
    JsonLib
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        LINKER_LANGUAGE CXX
        SOVERSION 1
        VERSION 1.0.0
)

target_include_directories(
    JsonLib
    PUBLIC
        json
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
    Camera.h
    Camera.cpp
    Mesh.h
    Mesh.cpp
    Model.h
    Model.cpp
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
        StbLib
        JsonLib
        glfw
        GLEW::GLEW
        OpenGL::GL
)
