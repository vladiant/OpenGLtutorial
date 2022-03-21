#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <png++/png.hpp>
#include <vector>

const unsigned int SCREEN_WIDTH = 720;
const unsigned int SCREEN_HEIGHT = 720;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

bool vSync = true;

GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
    0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
};

GLuint indices[] = {0, 2, 1, 0, 3, 2};

const char* vertexShaderSource = R"(#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
out vec2 UVs;
void main()
{
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.000);
	UVs = uvs;
})";
const char* fragmentShaderSource = R"(#version 460 core
out vec4 FragColor;
uniform sampler2D tex;
in vec2 UVs;
void main()
{
	FragColor = vec4(0.965, 0.318, 0.000, 1.000);
	FragColor = texture(tex, UVs);
})";

const char* framebufferVertexShaderSource = R"(#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
out vec2 UVs;
void main()
{
	gl_Position = vec4(2.0 * pos.x, 2.0 * pos.y, 2.0 * pos.z, 1.000);
	UVs = uvs;
})";
const char* framebufferFragmentShaderSource = R"(#version 460 core
out vec4 FragColor;
uniform sampler2D screen;
in vec2 UVs;
void main()
{
	FragColor = vec4(1.0) - texture(screen, UVs);
})";

int main() {
  if (!glfwInit()) return EXIT_FAILURE;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                                        "OpenGL Context", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create the GLFW window\n";
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(vSync);

  // Load GLEW so it configures OpenGL
  glewInit();

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLuint VAO, VBO, EBO;
  glCreateVertexArrays(1, &VAO);
  glCreateBuffers(1, &VBO);
  glCreateBuffers(1, &EBO);

  glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

  glEnableVertexArrayAttrib(VAO, 0);
  glVertexArrayAttribBinding(VAO, 0, 0);
  glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

  glEnableVertexArrayAttrib(VAO, 1);
  glVertexArrayAttribBinding(VAO, 1, 0);
  glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
  glVertexArrayElementBuffer(VAO, EBO);

  png::image<png::rgb_pixel> image("hamster.png");

  int widthImg = image.get_width();
  int heightImg = image.get_height();

  // Load the image into the texture
  std::vector<uint8_t> buf(image.get_width() * image.get_height() * 3);

  ssize_t size_read = 0;
  for (size_t y = 0; y < image.get_height(); ++y) {
    for (size_t x = 0; x < image.get_width(); ++x) {
      buf[size_read++] = image[y][x].red;
      buf[size_read++] = image[y][x].green;
      buf[size_read++] = image[y][x].blue;
    }
  }

  GLuint tex;
  glCreateTextures(GL_TEXTURE_2D, 1, &tex);

  glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTextureStorage2D(tex, 1, GL_RGBA8, widthImg, heightImg);
  glTextureSubImage2D(tex, 0, 0, 0, widthImg, heightImg, GL_RGB,
                      GL_UNSIGNED_BYTE, buf.data());
  glGenerateTextureMipmap(tex);

  GLuint FBO;
  glCreateFramebuffers(1, &FBO);

  GLuint framebufferTex;
  glCreateTextures(GL_TEXTURE_2D, 1, &framebufferTex);
  glTextureParameteri(framebufferTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(framebufferTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(framebufferTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(framebufferTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureStorage2D(framebufferTex, 1, GL_RGB8, SCREEN_WIDTH, SCREEN_HEIGHT);
  glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, framebufferTex, 0);

  auto fboStatus = glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer error: " << fboStatus << "\n";

  GLuint framebufferVertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(framebufferVertexShader, 1, &framebufferVertexShaderSource,
                 NULL);
  glCompileShader(framebufferVertexShader);
  GLuint framebufferFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(framebufferFragmentShader, 1, &framebufferFragmentShaderSource,
                 NULL);
  glCompileShader(framebufferFragmentShader);

  GLuint framebufferShaderProgram = glCreateProgram();
  glAttachShader(framebufferShaderProgram, framebufferVertexShader);
  glAttachShader(framebufferShaderProgram, framebufferFragmentShader);
  glLinkProgram(framebufferShaderProgram);

  glDeleteShader(framebufferVertexShader);
  glDeleteShader(framebufferFragmentShader);

  while (!glfwWindowShouldClose(window)) {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    GLfloat backgroundColor[] = {19.0f / 255.0f, 34.0f / 255.0f, 44.0f / 255.0f,
                                 1.0f};
    glClearNamedFramebufferfv(FBO, GL_COLOR, 0, backgroundColor);

    glUseProgram(shaderProgram);
    glBindTextureUnit(0, tex);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),
                   GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUseProgram(framebufferShaderProgram);
    glBindTextureUnit(0, framebufferTex);
    glUniform1i(glGetUniformLocation(framebufferShaderProgram, "screen"), 0);
    glBindVertexArray(VAO);  // NO framebuffer VAO because I simply double the
                             // size of the rectangle to cover the whole screen
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),
                   GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
