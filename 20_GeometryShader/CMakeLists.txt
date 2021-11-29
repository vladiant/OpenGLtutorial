cmake_minimum_required(VERSION 3.10)

project(20_GeometryShader LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

configure_file(default.frag . COPYONLY)
configure_file(default.vert . COPYONLY)
configure_file(default.geom . COPYONLY)
configure_file(normals.frag . COPYONLY)
configure_file(normals.geom . COPYONLY)
# Model data
configure_file(./statue/170516_mia337_032122_600_200Kfaces_8192px_OBJ_baseColor.jpg ./statue COPYONLY)
configure_file(./statue/scene.bin ./statue COPYONLY)
configure_file(./statue/scene.gltf ./statue COPYONLY)


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
