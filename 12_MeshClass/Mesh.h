#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>

#include "Camera.h"
#include "EBO.h"
#include "Texture.h"
#include "VAO.h"

class Mesh {
 public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;
  // Store VAO in public so it can be used in the Draw function
  VAO vao;

  // Initializes the mesh
  Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
       std::vector<Texture>& textures);

  // Draws the mesh
  void Draw(Shader& shader, Camera& camera);
};
#endif