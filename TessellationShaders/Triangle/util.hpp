#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cerrno>
#include <cstdio>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>

// Window variables
GLFWwindow* window;
const std::string windowName = "Tessellation";
const unsigned int SCREEN_WIDTH = 720;
const unsigned int SCREEN_HEIGHT = 720;

// Rendering settings
bool vSync = true;
bool wireframe = true;

// Shader Programs
GLuint shaderProgram;

// Textures
GLuint screen;

// Variables used to determine framerate
double previousUpdateTime = 0.0;
double currentUpdateTime = 0.0;
double timeDeltaFramerate = 0.0;
int framesCounter = 0;
int avgCounter = 0;
double fpsTotal = 0.0;
double msTotal = 0.0;

// Variables used for logic
double timeDelta = 0.0;
double previousTime = 0.0;

std::vector<GLfloat> genNonSymPlaneUniform(glm::vec3 v0, glm::vec3 v1,
                                           glm::vec3 v2, glm::vec3 v3,
                                           int div) {
  std::vector<GLfloat> plane;

  glm::vec3 dir03 = (v3 - v0) / float(div);
  glm::vec3 dir12 = (v2 - v1) / float(div);

  // dir2 and dir3
  for (int i = 0; i < div + 1; i++) {
    // dir1
    for (int j = 0; j < div + 1; j++) {
      glm::vec3 acrossj =
          ((v1 + float(i) * dir12) - (v0 + float(i) * dir03)) / float(div);
      glm::vec3 crntVec = v0 + float(i) * dir03 + float(j) * acrossj;
      // Position
      plane.push_back(crntVec.x);
      plane.push_back(crntVec.y);
      plane.push_back(crntVec.z);
      // Tex UV
      plane.push_back(float(j) / div);
      plane.push_back(float(i) / div);
    }
  }

  return plane;
}

// Plane indices for triangle patches
std::vector<GLuint> genPlaneInd(int div) {
  std::vector<GLuint> indices;

  for (int row = 0; row < div; row++) {
    for (int col = 0; col < div; col++) {
      int index = row * (div + 1) + col;
      // Top triangle ____
      indices.push_back(index);                  // |  /
      indices.push_back(index + (div + 1) + 1);  // | /
      indices.push_back(index + (div + 1));      // |/
      // Bot triangle
      indices.push_back(index);                  //   /|
      indices.push_back(index + 1);              //	/ |
      indices.push_back(index + (div + 1) + 1);  // /__|
    }
  }

  return indices;
}

// Plane indices for quad patches
std::vector<GLuint> genPlaneIndTes(int div) {
  std::vector<GLuint> indices;

  for (int row = 0; row < div; row++) {
    for (int col = 0; col < div; col++) {
      int index = row * (div + 1) + col;         // 3___2
      indices.push_back(index);                  //     |
      indices.push_back(index + 1);              //     |
      indices.push_back(index + (div + 1) + 1);  //  ___|
      indices.push_back(index + (div + 1));      // 0   1
    }
  }

  return indices;
}

// Screen mesh
GLuint VAO, VBO, EBO;

// Triangle
std::vector<GLfloat> vertices = {
    -0.6f, -0.3f, 0.0f, 0.0f, 0.0f, 0.6f, -0.3f, 0.0f,
    1.0f,  0.0f,  0.0f, 0.6f, 0.0f, 0.5f, 1.0f,
};
std::vector<GLuint> indices = {0, 1, 2};

void init() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void createContext() {
  window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowName.c_str(),
                            NULL, NULL);
  if (!window) {
    std::cout << "Failed to create the GLFW window\n";
    glfwTerminate();
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(vSync);

  glewInit();
}

void glEnableConfig() {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
}

void displayStats() {
  currentUpdateTime = glfwGetTime();
  timeDeltaFramerate = currentUpdateTime - previousUpdateTime;
  framesCounter++;

  if (timeDeltaFramerate >= 1.0 / 10.0 && currentUpdateTime > 5.0) {
    double fps = framesCounter / timeDeltaFramerate;
    double ms = timeDeltaFramerate / framesCounter * 1000.0;

    fpsTotal += fps;
    msTotal += ms;
    avgCounter++;
    double avgfps = fpsTotal / avgCounter;
    double avgms = msTotal / avgCounter;

    std::string fpsStr = std::to_string((int)fps);
    std::string avgfpsStr = std::to_string((int)avgfps);
    std::string msStr = std::to_string(ms).substr(0, 6);
    std::string avgmsStr = std::to_string(avgms).substr(0, 6);
    std::string newWindowTitle = windowName + " - " + fpsStr + "(" + avgfpsStr +
                                 ")" + "FPS / " + msStr + "(" + avgmsStr + ")" +
                                 "ms";
    glfwSetWindowTitle(window, newWindowTitle.c_str());

    previousUpdateTime = currentUpdateTime;
    framesCounter = 0;
  }
}

void updateTimeDelta() {
  double currentTime = glfwGetTime();
  timeDelta = currentTime - previousTime;
  previousTime = currentTime;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (key == GLFW_KEY_V && action == GLFW_PRESS) {
    vSync = !vSync;
    glfwSwapInterval(vSync);
  }
  if (key == GLFW_KEY_C && action == GLFW_PRESS) wireframe = !wireframe;
}

void terminate(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

std::string readText(const char* textFile) {
  std::FILE* fp = std::fopen(textFile, "rb");
  if (fp) {
    std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, contents.size(), fp);
    std::fclose(fp);
    return (contents);
  }
  throw(errno);
}

void initRender() {
  std::string vertexCode = readText("vertex.vert");
  std::string fragmentCode = readText("fragment.frag");
  std::string tesControlCode = readText("tessellation.tesc");
  std::string tesEvalCode = readText("tessellation.tese");

  const char* vertexSource = vertexCode.c_str();
  const char* fragmentSource = fragmentCode.c_str();
  const char* tesControlSource = tesControlCode.c_str();
  const char* tesEvalSource = tesEvalCode.c_str();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  GLuint tesControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
  glShaderSource(tesControlShader, 1, &tesControlSource, NULL);
  glCompileShader(tesControlShader);
  GLuint tesEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
  glShaderSource(tesEvalShader, 1, &tesEvalSource, NULL);
  glCompileShader(tesEvalShader);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glAttachShader(shaderProgram, tesControlShader);
  glAttachShader(shaderProgram, tesEvalShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteShader(tesControlShader);
  glDeleteShader(tesEvalShader);

  glCreateVertexArrays(1, &VAO);
  glCreateBuffers(1, &VBO);
  glCreateBuffers(1, &EBO);

  glNamedBufferData(VBO, vertices.size() * sizeof(vertices[0]), vertices.data(),
                    GL_STATIC_DRAW);
  glNamedBufferData(EBO, indices.size() * sizeof(indices[0]), indices.data(),
                    GL_STATIC_DRAW);

  glEnableVertexArrayAttrib(VAO, 0);
  glVertexArrayAttribBinding(VAO, 0, 0);
  glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

  glEnableVertexArrayAttrib(VAO, 1);
  glVertexArrayAttribBinding(VAO, 1, 0);
  glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
  glVertexArrayElementBuffer(VAO, EBO);

  glCreateTextures(GL_TEXTURE_2D, 1, &screen);
  glTextureParameteri(screen, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(screen, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(screen, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(screen, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureStorage2D(screen, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);
  glBindImageTexture(0, screen, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
