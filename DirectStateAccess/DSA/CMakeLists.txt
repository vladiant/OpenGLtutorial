cmake_minimum_required(VERSION 3.10)

project(DSA LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

configure_file(hamster.png . COPYONLY)

add_executable(
    ${PROJECT_NAME}
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
        PNG::PNG
)
