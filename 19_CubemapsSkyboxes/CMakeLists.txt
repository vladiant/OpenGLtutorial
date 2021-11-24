cmake_minimum_required(VERSION 3.10)

project(19_CubemapsSkyboxes LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

configure_file(default.frag . COPYONLY)
configure_file(default.vert . COPYONLY)
configure_file(skybox.frag . COPYONLY)
configure_file(skybox.vert . COPYONLY)
# Model data
configure_file(./airplane/diffuse.png ./airplane COPYONLY)
configure_file(./airplane/specular.png ./airplane COPYONLY)
configure_file(./airplane/scene.bin ./airplane COPYONLY)
configure_file(./airplane/scene.gltf ./airplane COPYONLY)
# Skybox
configure_file(./skybox/back.jpg ./skybox COPYONLY)
configure_file(./skybox/bottom.jpg ./skybox COPYONLY)
configure_file(./skybox/front.jpg ./skybox COPYONLY)
configure_file(./skybox/left.jpg ./skybox COPYONLY)
configure_file(./skybox/right.jpg ./skybox COPYONLY)
configure_file(./skybox/top.jpg ./skybox COPYONLY)



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
