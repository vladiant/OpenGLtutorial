//------- Ignore this ----------
#include <filesystem>
namespace fs = std::filesystem;
//------------------------------

#include "Model.h"

const unsigned int width = 800;
const unsigned int height = 800;

// Number of samples per pixel for MSAA
unsigned int samples = 8;

// Controls the gamma function
float gammaValue = 2.2f;

float rectangleVertices[] = {
    //  Coords   // texCoords
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};

int main() {
  // Initialize GLFW
  glfwInit();

  // Tell GLFW what version of OpenGL we are using
  // In this case we are using OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Only use this if you don't have a framebuffer
  // glfwWindowHint(GLFW_SAMPLES, samples);
  // Tell GLFW we are using the CORE profile
  // So that means we only have the modern functions
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
  GLFWwindow* window =
      glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
  // Error check if the window fails to create
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  // Introduce the window into the current context
  glfwMakeContextCurrent(window);

  // Load GLEW so it configures OpenGL
  glewInit();
  // Specify the viewport of OpenGL in the Window
  // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
  glViewport(0, 0, width, height);

  // Generates shaders
  Shader shaderProgram("default.vert", "default.frag");
  Shader framebufferProgram("framebuffer.vert", "framebuffer.frag");
  Shader shadowMapProgram("shadowMap.vert", "shadowMap.frag");
  Shader shadowCubeMapProgram("shadowCubeMap.vert", "shadowCubeMap.frag",
                              "shadowCubeMap.geom");

  // Take care of all the light related things
  glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  glm::vec3 lightPos = glm::vec3(0.0f, 10.0f, 0.0f);

  shaderProgram.Activate();
  glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"),
              lightColor.x, lightColor.y, lightColor.z, lightColor.w);
  glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x,
              lightPos.y, lightPos.z);
  framebufferProgram.Activate();
  glUniform1i(glGetUniformLocation(framebufferProgram.ID, "screenTexture"), 0);
  glUniform1f(glGetUniformLocation(framebufferProgram.ID, "gamma"), gammaValue);

  // Enables the Depth Buffer
  glEnable(GL_DEPTH_TEST);

  // Enables Multisampling
  glEnable(GL_MULTISAMPLE);

  // Enables Cull Facing
  glEnable(GL_CULL_FACE);
  // Keeps front faces
  glCullFace(GL_FRONT);
  // Uses counter clock-wise standard
  glFrontFace(GL_CCW);

  // Creates camera object
  Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

  /*
   * I'm doing this relative path thing in order to centralize all the resources
   * into one folder and not duplicate them between tutorial folders. You can
   * just copy paste the resources from the 'Resources' folder and then give a
   * relative path from this folder to whatever resource you want to get to.
   * Also note that this requires C++17, so go to Project Properties, C/C++,
   * Language, and select C++17
   */
  std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
  std::string groundPath = "/ground/scene.gltf";
  std::string treesPath = "/trees/scene.gltf";

  // Load in models
  Model ground((parentDir + groundPath).c_str());
  Model trees((parentDir + treesPath).c_str());

  // Prepare framebuffer rectangle VBO and VAO
  unsigned int rectVAO, rectVBO;
  glGenVertexArrays(1, &rectVAO);
  glGenBuffers(1, &rectVBO);
  glBindVertexArray(rectVAO);
  glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void*)(2 * sizeof(float)));

  // Variables to create periodic event for FPS displaying
  double prevTime = 0.0;
  double crntTime = 0.0;
  double timeDiff;
  // Keeps track of the amount of frames in timeDiff
  unsigned int counter = 0;

  // Use this to disable VSync (not advized)
  // glfwSwapInterval(0);

  // Create Frame Buffer Object
  unsigned int FBO;
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  // Create Framebuffer Texture
  unsigned int framebufferTexture;
  glGenTextures(1, &framebufferTexture);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, width,
                          height, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);  // Prevents edge bleeding
  glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE);  // Prevents edge bleeding
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D_MULTISAMPLE, framebufferTexture, 0);

  // Create Render Buffer Object
  unsigned int RBO;
  glGenRenderbuffers(1, &RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
                                   GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, RBO);

  // Error checking framebuffer
  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer error: " << fboStatus << std::endl;

  // Create Frame Buffer Object
  unsigned int postProcessingFBO;
  glGenFramebuffers(1, &postProcessingFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

  // Create Framebuffer Texture
  unsigned int postProcessingTexture;
  glGenTextures(1, &postProcessingTexture);
  glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         postProcessingTexture, 0);

  // Error checking framebuffer
  fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Post-Processing Framebuffer error: " << fboStatus
              << std::endl;

  // Framebuffer for Shadow Map
  unsigned int shadowMapFBO;
  glGenFramebuffers(1, &shadowMapFBO);

  // Texture for Shadow Map FBO
  unsigned int shadowMapWidth = 2048, shadowMapHeight = 2048;
  unsigned int shadowMap;
  glGenTextures(1, &shadowMap);
  glBindTexture(GL_TEXTURE_2D, shadowMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth,
               shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  // Prevents darkness outside the frustrum
  float clampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadowMap, 0);
  // Needed since we don't touch the color buffer
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Matrices needed for the light's perspective
  float farPlane = 100.0f;
  glm::mat4 orthgonalProjection =
      glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, farPlane);
  glm::mat4 perspectiveProjection =
      glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
  glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f),
                                    glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 lightProjection = perspectiveProjection * lightView;

  shadowMapProgram.Activate();
  glUniformMatrix4fv(
      glGetUniformLocation(shadowMapProgram.ID, "lightProjection"), 1, GL_FALSE,
      glm::value_ptr(lightProjection));

  // Framebuffer for Cubemap Shadow Map
  unsigned int pointShadowMapFBO;
  glGenFramebuffers(1, &pointShadowMapFBO);

  // Texture for Cubemap Shadow Map FBO
  unsigned int depthCubemap;
  glGenTextures(1, &depthCubemap);

  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
  for (unsigned int i = 0; i < 6; ++i)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Matrices needed for the light's perspective on all faces of the cubemap
  glm::mat4 shadowProj =
      glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
  glm::mat4 shadowTransforms[] = {
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0),
                               glm::vec3(0.0, -1.0, 0.0)),
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0),
                               glm::vec3(0.0, -1.0, 0.0)),
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0),
                               glm::vec3(0.0, 0.0, 1.0)),
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0),
                               glm::vec3(0.0, 0.0, -1.0)),
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0),
                               glm::vec3(0.0, -1.0, 0.0)),
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0),
                               glm::vec3(0.0, -1.0, 0.0))};
  // Export all matrices to shader
  shadowCubeMapProgram.Activate();
  glUniformMatrix4fv(
      glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[0]"), 1,
      GL_FALSE, glm::value_ptr(shadowTransforms[0]));
  glUniformMatrix4fv(
      glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[1]"), 1,
      GL_FALSE, glm::value_ptr(shadowTransforms[1]));
  glUniformMatrix4fv(
      glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[2]"), 1,
      GL_FALSE, glm::value_ptr(shadowTransforms[2]));
  glUniformMatrix4fv(
      glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[3]"), 1,
      GL_FALSE, glm::value_ptr(shadowTransforms[3]));
  glUniformMatrix4fv(
      glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[4]"), 1,
      GL_FALSE, glm::value_ptr(shadowTransforms[4]));
  glUniformMatrix4fv(
      glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[5]"), 1,
      GL_FALSE, glm::value_ptr(shadowTransforms[5]));
  glUniform3f(glGetUniformLocation(shadowCubeMapProgram.ID, "lightPos"),
              lightPos.x, lightPos.y, lightPos.z);
  glUniform1f(glGetUniformLocation(shadowCubeMapProgram.ID, "farPlane"),
              farPlane);

  // Main while loop
  while (!glfwWindowShouldClose(window)) {
    // Updates counter and times
    crntTime = glfwGetTime();
    timeDiff = crntTime - prevTime;
    counter++;

    if (timeDiff >= 1.0 / 30.0) {
      // Creates new title
      std::string FPS = std::to_string((1.0 / timeDiff) * counter);
      std::string ms = std::to_string((timeDiff / counter) * 1000);
      std::string newTitle = "YoutubeOpenGL - " + FPS + "FPS / " + ms + "ms";
      glfwSetWindowTitle(window, newTitle.c_str());

      // Resets times and counter
      prevTime = crntTime;
      counter = 0;

      // Use this if you have disabled VSync
      // camera.Inputs(window);
    }

    // Depth testing needed for Shadow Map
    glEnable(GL_DEPTH_TEST);

    // Preparations for the Shadow Map
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    // Commented code is for Spotlights and Directional Lights
    // glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    // glClear(GL_DEPTH_BUFFER_BIT);

    //// Draw scene for shadow map
    // ground.Draw(shadowMapProgram, camera);
    // trees.Draw(shadowMapProgram, camera);

    // Code for Point Lights
    glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw scene for shadow map
    ground.Draw(shadowCubeMapProgram, camera);
    trees.Draw(shadowCubeMapProgram, camera);

    // Switch back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Switch back to the default viewport
    glViewport(0, 0, width, height);
    // Bind the custom framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // Specify the color of the background
    glClearColor(pow(0.07f, gammaValue), pow(0.13f, gammaValue),
                 pow(0.17f, gammaValue), 1.0f);
    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable depth testing since it's disabled when drawing the framebuffer
    // rectangle
    glEnable(GL_DEPTH_TEST);

    // Handles camera inputs (delete this if you have disabled VSync)
    camera.Inputs(window);
    // Updates and exports the camera matrix to the Vertex Shader
    camera.updateMatrix(45.0f, 0.1f, farPlane);

    // Send the light matrix to the shader
    shaderProgram.Activate();
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram.ID, "lightProjection"), 1, GL_FALSE,
        glm::value_ptr(lightProjection));
    glUniform1f(glGetUniformLocation(shaderProgram.ID, "farPlane"), farPlane);

    // Bind the Shadow Map
    /*glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowMap"), 2);*/

    // Bind the Cubemap Shadow Map
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowCubeMap"), 2);

    // Draw the normal model
    ground.Draw(shaderProgram, camera);
    trees.Draw(shaderProgram, camera);

    // Make it so the multisampling FBO is read while the post-processing FBO is
    // drawn
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO);
    // Conclude the multisampling and copy it to the post-processing FBO
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Draw the framebuffer rectangle
    framebufferProgram.Activate();
    glBindVertexArray(rectVAO);
    glDisable(
        GL_DEPTH_TEST);  // prevents framebuffer rectangle from being discarded
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Swap the back buffer with the front buffer
    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
  }

  // Delete all the objects we've created
  shaderProgram.Delete();
  glDeleteFramebuffers(1, &FBO);
  glDeleteFramebuffers(1, &postProcessingFBO);
  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
  return 0;
}
