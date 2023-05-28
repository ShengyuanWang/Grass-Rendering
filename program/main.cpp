/*
* Kaiyang Yao, Shengyuan Wang
*
* Reference: https://vulpinii.github.io/tutorials/grass-modelisation/en/
* https://learnopengl.com/Advanced-OpenGL/Cubemaps
*/

// Include standard headers
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>
// Include GLEW, GLFW, GLM
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Include shader
#include <common/Shader.hpp>
#include <common/stb_image.hpp>
#define STB_IMAGE_IMPLEMENTATION
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, -0.5f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

int textureCount = 0;

int main(void) {
  /***************************************
   * Initialization
   ***************************************/

  // Initialize GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grass Simulation!", NULL, NULL);

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Initialize GLEW
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Initialize background
  glClearColor(0.215f, 0.215f, 0.215f, 1.0f);
  glfwSwapInterval(0);
  glEnable(GL_DEPTH_TEST);


  /***************************************
   * Grass
   ***************************************/
  GLuint grassShader =
      load_shaders("../../assets/shaders/grass.vs", "../../assets/shaders/grass.fs",
                   "../../assets/shaders/grass.gs");
  glUseProgram(grassShader);
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
  glUniformMatrix4fv(glGetUniformLocation(grassShader, "u_projection"), 1, GL_FALSE, &projection[0][0]);
  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  glUniformMatrix4fv(glGetUniformLocation(grassShader, "u_view"), 1, GL_FALSE, &view[0][0]);

  std::vector<glm::vec3> positions;
  for (float x = -10.0f; x < 10.0f; x += 0.06f) {
    for (float z = -10.0f; z < 10.0f; z += 0.06f) {
      positions.push_back(glm::vec3(x, -1, z));
    }
  }

  unsigned int grassVAO, grassVBO;
  glGenVertexArrays(1, &grassVAO);
  glBindVertexArray(grassVAO);

  glGenBuffers(1, &grassVBO);
  glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(grassShader);

  unsigned int grassTexture1 = loadTexture("../../assets/textures/grass.png");
  glUniform1i(glGetUniformLocation(grassShader, "u_grassTexture1"), textureCount++);


  /***************************************
   * Land
   ***************************************/
  GLuint landShader = load_shaders("../../assets/shaders/land.vs", "../../assets/shaders/land.fs");
  glUseProgram(landShader);
  glUniformMatrix4fv(glGetUniformLocation(landShader, "u_projection"), 1, GL_FALSE, &projection[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(landShader, "u_view"), 1, GL_FALSE, &view[0][0]);

  float landVertices[] = {
      // Vertices           // Textures
      -10.0f, -1.0f, -10.0f, 0.0f, 0.0f, // down left
      10.0f,  -1.0f, -10.0f, 1.0f, 0.0f, // down right
      -10.0f, -1.0f, 10.0f,  0.0f, 1.0f, // up left
      10.0f,  -1.0f, 10.0f,  1.0f, 1.0f  // up right
  };

  float repeatFactor = 100.0f;
  for (int i = 0; i < 4; i++) {
    landVertices[i * 5 + 3] *= repeatFactor;
    landVertices[i * 5 + 4] *= repeatFactor;
  }

  unsigned int landVAO, landVBO;
  glGenVertexArrays(1, &landVAO);
  glBindVertexArray(landVAO);

  glGenBuffers(1, &landVBO);
  glBindBuffer(GL_ARRAY_BUFFER, landVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(landVertices), landVertices, GL_STATIC_DRAW);

  // push position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // push texture
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  unsigned int landTexture = loadTexture("../../assets/textures/land.png");
  glUniform1i(glGetUniformLocation(landShader, "u_landTexture"), textureCount++);


  /***************************************
   * Sky Box
   ***************************************/
  GLuint skyboxShader = load_shaders("../../assets/shaders/skybox.vs", "../../assets/shaders/skybox.fs");
  glUseProgram(skyboxShader);
  glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "u_projection"), 1, GL_FALSE,
                     &projection[0][0]);
  glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "u_view"), 1, GL_FALSE, &view[0][0]);

  float skyboxVertices[] = {-15.0f, 15.0f,  -15.0f, -15.0f, -15.0f, -15.0f, 15.0f,  -15.0f, -15.0f,
                            15.0f,  -15.0f, -15.0f, 15.0f,  15.0f,  -15.0f, -15.0f, 15.0f,  -15.0f,

                            -15.0f, -15.0f, 15.0f,  -15.0f, -15.0f, -15.0f, -15.0f, 15.0f,  -15.0f,
                            -15.0f, 15.0f,  -15.0f, -15.0f, 15.0f,  15.0f,  -15.0f, -15.0f, 15.0f,

                            15.0f,  -15.0f, -15.0f, 15.0f,  -15.0f, 15.0f,  15.0f,  15.0f,  15.0f,
                            15.0f,  15.0f,  15.0f,  15.0f,  15.0f,  -15.0f, 15.0f,  -15.0f, -15.0f,

                            -15.0f, -15.0f, 15.0f,  -15.0f, 15.0f,  15.0f,  15.0f,  15.0f,  15.0f,
                            15.0f,  15.0f,  15.0f,  15.0f,  -15.0f, 15.0f,  -15.0f, -15.0f, 15.0f,

                            -15.0f, 15.0f,  -15.0f, 15.0f,  15.0f,  -15.0f, 15.0f,  15.0f,  15.0f,
                            15.0f,  15.0f,  15.0f,  -15.0f, 15.0f,  15.0f,  -15.0f, 15.0f,  -15.0f,

                            -15.0f, -15.0f, -15.0f, -15.0f, -15.0f, 15.0f,  15.0f,  -15.0f, -15.0f,
                            15.0f,  -15.0f, -15.0f, -15.0f, -15.0f, 15.0f,  15.0f,  -15.0f, 15.0f};

  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glBindVertexArray(skyboxVAO);

  glGenBuffers(1, &skyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  vector<std::string> faces;
  faces.push_back("../../assets/textures/skybox/grass_right.png");
  faces.push_back("../../assets/textures/skybox/grass_left.png");
  faces.push_back("../../assets/textures/skybox/grass_up.png");
  faces.push_back("../../assets/textures/skybox/grass_down.png");
  faces.push_back("../../assets/textures/skybox/grass_front.png");
  faces.push_back("../../assets/textures/skybox/grass_back.png");
  unsigned int skyboxTexture = loadCubemap(faces);


  /***************************************
   * Bind Textures
   ***************************************/
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, grassTexture1);
  // For adding more grass/flower textures
  // glActiveTexture(GL_TEXTURE1);
  // glBindTexture(GL_TEXTURE_2D, grassTexture2);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, landTexture);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, skyboxTexture);


  /***************************************
   * Render Loop
   ***************************************/
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // input
    processInput(window);

    // reset color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update view
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // draw
    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to
                            // depth buffer's content
    glUseProgram(skyboxShader);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "u_projection"), 1, GL_FALSE,
                       &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "u_view"), 1, GL_FALSE, &view[0][0]);
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    glUseProgram(landShader);
    glUniformMatrix4fv(glGetUniformLocation(grassShader, "u_view"), 1, GL_FALSE, &view[0][0]);
    glUniform3fv(glGetUniformLocation(grassShader, "u_cameraPosition"), 1, &cameraPos[0]);
    glUniformMatrix4fv(glGetUniformLocation(landShader, "u_view"), 1, GL_FALSE, &view[0][0]);
    glUniform3fv(glGetUniformLocation(landShader, "u_cameraPosition"), 1, &cameraPos[0]);
    glBindVertexArray(landVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(grassShader);
    glUniform1f(glGetUniformLocation(grassShader, "u_time"), glfwGetTime());
    glUniformMatrix4fv(glGetUniformLocation(grassShader, "u_view"), 1, GL_FALSE, &view[0][0]);
    glUniform3fv(glGetUniformLocation(grassShader, "u_cameraPosition"), 1, &cameraPos[0]);
    glUniformMatrix4fv(glGetUniformLocation(landShader, "u_view"), 1, GL_FALSE, &view[0][0]);
    glUniform3fv(glGetUniformLocation(landShader, "u_cameraPosition"), 1, &cameraPos[0]);
    glBindVertexArray(grassVAO);
    glDrawArrays(GL_POINTS, 0, positions.size());

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  /***************************************
   * Clean Up
   ***************************************/
  glDeleteVertexArrays(1, &grassVAO);
  glDeleteBuffers(1, &grassVBO);
  glDeleteProgram(grassShader);

  glDeleteVertexArrays(1, &landVAO);
  glDeleteBuffers(1, &landVBO);
  glDeleteProgram(landShader);

  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteBuffers(1, &skyboxVBO);
  glDeleteProgram(skyboxShader);

  glfwTerminate();
  return 0;
}


/***************************************
 * Helper Functions
 ***************************************/
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = 2.5 * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cameraPos += cameraUp * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cameraPos -= cameraUp * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  fov -= (float)yoffset;
  if (fov < 1.0f)
    fov = 1.0f;
  if (fov > 45.0f)
    fov = 45.0f;
}

unsigned int loadTexture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrComponents;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  stbi_set_flip_vertically_on_load(false);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }

  stbi_image_free(data);

  return textureID;
}

unsigned int loadCubemap(vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

// Archived function
vector<unsigned int> loadTexturesFromFiles(const vector<string> &paths) {
  vector<unsigned int> textures;

  for (const auto &path : paths) {
    std::string filename = path;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    stbi_set_flip_vertically_on_load(false);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      textures.push_back(texture);
    } else {
      std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    stbi_image_free(data);
  }

  return textures;
}
