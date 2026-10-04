#pragma once
#include <iostream>
#include <cstddef>
#include <array>
#include <cassert>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "../../../dependencies/stb_image/include/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



void clear_gl_errors() {
  while (glGetError() != GL_NO_ERROR) {}
}

void log_gl_errors(const char* expression, std::size_t line, const char* file_name) {

  int error_code = glGetError();
  bool loop_ran = false;

  while (error_code != GL_NO_ERROR) {
    std::cout << "OpenGL Error on: " << expression
      << "\nBreakpoint trigger:"
      << "\nAt line: " << line
      << "\nFile: " << file_name
      << "\nError Code: " << error_code;
    error_code = glGetError();
    loop_ran = true;

  }

  if (loop_ran) void(); // [!!!!! CRITICAL !!!!!] 
  //PLEASE ADD A DEBUG BREAK, MAY BE A MACRO FOR DIFFERNT COMPILERS 
  // some opengl error occured // check call stack and terminal
}

#define VALIDATE_GL_(expr_gl) do {                             \
clear_gl_errors();								                             \
expr_gl;                                                       \
log_gl_errors(#expr_gl, __LINE__, __FILE__);                   \
} while(0)


class Camera {
  GLFWwindow* window;
  bool take_input = false;
  public:
  float pitch{};
  float yaw{-90.f};
  float lastX;
  float lastY;

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  

  float deltaTime{};
  float lastFrame{};


  private:
  bool firstmouse = false;

  public:
  
  Camera(GLFWwindow* window)
  : window(window)
  {

    glfwSetWindowUserPointer(window, this); // set the current camera object as the window user pointer

    int window_width{};
    int window_height{};

    glfwGetWindowSize(window, &window_width, &window_height);

    lastX = static_cast<float>(window_width) / 2.f;
    lastY = static_cast<float>(window_height) / 2.f;

  }

  ~Camera(){
    glfwSetWindowUserPointer(window, nullptr); // set nullptr as the window user pointer

  }



  void ProcessInput(double xpos, double ypos){
    if (!take_input) return;
    if (firstmouse){
    lastX = xpos;
    lastY = ypos;
    firstmouse = false;
    return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed: y ranges bottom to top
    lastX = xpos;
    lastY = ypos;
    const float sensitivity = 0.08f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  void SetCameraEnabled(){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disabling cursor... not camera
    firstmouse = true;
    take_input = true;
  }

  void SetCameraDisabled(){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    firstmouse = false; // only set to false for understanding and clarity
    // is set to false by mouse_callback
    take_input = false;
  }

  void OnOpenGLRender(){ // to be ivoked everyframe
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
  }

  glm::mat4 GetViewMatrix() { // to be invoked everyframe
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  }

  void predefined_generic_WASD_control(){
    
    const float cameraSpeed = deltaTime * 3.0f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      SetCameraDisabled();
    // SetCameraMode(glfw_window, false);

    
  }


};




void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
  
  if (glfwGetWindowUserPointer(window))
  static_cast<Camera*>(glfwGetWindowUserPointer(window))->ProcessInput(xpos, ypos);


  // if (firstmouse){
  //   lastX = xpos;
  //   lastY = ypos;
  //   firstmouse = false;
  //   return;
  // }

  // float xoffset = xpos - lastX;
  // float yoffset = lastY - ypos; // reversed: y ranges bottom to top
  // lastX = xpos;
  // lastY = ypos;
  // const float sensitivity = 0.08f;
  // xoffset *= sensitivity;
  // yoffset *= sensitivity;

  // yaw += xoffset;
  // pitch += yoffset;
  // if(pitch > 89.0f)
  // pitch = 89.0f;
  // if(pitch < -89.0f)
  // pitch = -89.0f;
  // //========================
  // std::cout << "Pitch: " << pitch << std::endl;
  // std::cout << "Yaw  : " << yaw << std::endl;


}



void processInput(GLFWwindow* window) {
  //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    //glfwSetWindowShouldClose(window, true);
  //}
}

unsigned int CreateShader(unsigned int shader_type_gl, char const* const* const shader_src_string_array, unsigned int src_count) {
  unsigned int shader;
  shader = glCreateShader(shader_type_gl);

  VALIDATE_GL_(glShaderSource(shader, src_count, shader_src_string_array, nullptr));
  VALIDATE_GL_(glCompileShader(shader));

  int success;
  char infolog[512];
  VALIDATE_GL_(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));

  if (!success) {
    VALIDATE_GL_(glGetShaderInfoLog(shader, 512, nullptr, infolog));
    std::cout << "Error compiling shader: \n" << infolog << std::endl;
  }

  return shader;
}

unsigned int CreateShaderProgram(unsigned int VertexShader, unsigned int FragmentShader) {
  unsigned int ShaderProgram;
  ShaderProgram = glCreateProgram();

  VALIDATE_GL_(glAttachShader(ShaderProgram, VertexShader));
  VALIDATE_GL_(glAttachShader(ShaderProgram, FragmentShader));
  VALIDATE_GL_(glLinkProgram(ShaderProgram));

  int success;
  char infolog[512];

  VALIDATE_GL_(glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success));
  if (!success) {
    VALIDATE_GL_(glGetProgramInfoLog(ShaderProgram, 512, nullptr, infolog));
    std::cout << "Error linking : \n" << infolog << std::endl;
  }
  return ShaderProgram;
}




class VertexArray {
private:
  unsigned int VAO;
public:
  
  VertexArray() {
    VALIDATE_GL_(glGenVertexArrays(1, &VAO));
  }
  
  void bind() const {
    VALIDATE_GL_(glBindVertexArray(VAO));
  }

  void push_arrtib_pointer(std::size_t index, std::size_t element_count_per_vertex_arrtib, unsigned int type_type_GLenum,
    int normalise, std::size_t stride_size_bytes, std::size_t vertex_element_offset_size_bytes) {
    VALIDATE_GL_(glVertexAttribPointer(

      index, element_count_per_vertex_arrtib, type_type_GLenum, normalise, stride_size_bytes, reinterpret_cast<void*>(vertex_element_offset_size_bytes)

    ));

  }
  
  void enable(std::size_t index) {
    VALIDATE_GL_(glEnableVertexAttribArray(index));
  }

  unsigned int id() const{
    return VAO;
  }

};

class VertexBuffer {
private:
  unsigned int VBO;
public:
  
  VertexBuffer() {
    VALIDATE_GL_(glGenBuffers(1, &VBO));
  }

  void bind(unsigned int target_type_GLenum) {
    VALIDATE_GL_(glBindBuffer(target_type_GLenum, VBO));
  }
  
  template <typename T, std::size_t buffer_size>
  void set_data(unsigned int target_type_GLenum, const std::array<T, buffer_size>& buffer, unsigned int usage_type_GLenum) {
    VALIDATE_GL_(glBufferData(target_type_GLenum, sizeof(T) * buffer_size, buffer.data(), usage_type_GLenum));
  }

  unsigned int id() const{
    return VBO;
  }
};

class ShaderVertex {

private:
  unsigned int _id;

public:
  ShaderVertex() : _id(0) {
  }
  
  ShaderVertex(const char* const* const shader_src_string_array, std::size_t str_array_size = 1) {
    _id = CreateShader(GL_VERTEX_SHADER, shader_src_string_array, str_array_size);
  }

  void CompileShader_seperately(const char* const* const shader_src_string_array, std::size_t str_array_size) {
    assert(_id == 0 && "id already has a compiled shader");
    _id = CreateShader(GL_VERTEX_SHADER, shader_src_string_array, str_array_size);
  }


  unsigned int id() const{
    return _id;
  }
};

class ShaderFragment {

private:
  unsigned int _id;

public:
  ShaderFragment() : _id(0) {
  }

  ShaderFragment(const char* const* const shader_src_string_array, std::size_t str_array_size = 1) {
    _id = CreateShader(GL_FRAGMENT_SHADER, shader_src_string_array, str_array_size);
  }

  void CompileShader_seperately(const char* const* const shader_src_string_array, std::size_t str_array_size) {
    assert(_id == 0 && "id already has a compiled shader");
    _id = CreateShader(GL_FRAGMENT_SHADER, shader_src_string_array, str_array_size);
  }

  unsigned int id() const{
    return _id;
  }
};

class ElementBuffer {
private:
  unsigned int EBO;

public:
  ElementBuffer() {
    VALIDATE_GL_(glGenBuffers(1, &EBO));
  }

  template <typename T, std::size_t buffer_size>
  void set_data(unsigned int target_type_GLenum, const std::array<T, buffer_size>& buffer, unsigned int usage_type_GLenum) {
    VALIDATE_GL_(glBufferData(target_type_GLenum, sizeof(T) * buffer_size, buffer.data(), usage_type_GLenum));
  }
  
  void bind() {
    VALIDATE_GL_(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
  }

  unsigned int id() const {
    return EBO;
  }
}; 

class ShaderProgram {
private:
  unsigned int _id;
  std::unordered_map<std::string, int> uniform_location_cache;

public:

  ShaderProgram() : _id(0) {

  }

  ShaderProgram(ShaderVertex VertexShader, ShaderFragment FragmentShader) {
    _id = CreateShaderProgram(VertexShader.id(), FragmentShader.id());
  }

  void CreateProgram_seperately(ShaderVertex VertexShader, ShaderFragment FragmentShader) {
    _id = CreateShaderProgram(VertexShader.id(), FragmentShader.id());
  }

  void Bind() {
    VALIDATE_GL_(glUseProgram(_id));
  }

  void UnBind() {
    VALIDATE_GL_(glUseProgram(0));
  }

  unsigned int id() {
    return _id;
  }

  void SetUniform1i(const std::string& uniform_name, int v0) {
    VALIDATE_GL_(glUniform1i(GetUniformLocation(uniform_name), v0));
  }

  void SetUniform2i(const std::string& uniform_name, int v0, int v1) {
    VALIDATE_GL_(glUniform2i(GetUniformLocation(uniform_name), v0, v1));
  }

  void SetUniform3i(const std::string& uniform_name, int v0, int v1, int v2) {
    VALIDATE_GL_(glUniform3i(GetUniformLocation(uniform_name), v0, v1, v2));
  }

  void SetUniform4i(const std::string& uniform_name, int v0, int v1, int v2, int v3) {
    VALIDATE_GL_(glUniform4i(GetUniformLocation(uniform_name), v0, v1, v2, v3));
  }



  void SetUniform1f(const std::string& uniform_name, float v0) {
    VALIDATE_GL_(glUniform1f(GetUniformLocation(uniform_name), v0));
  }

  void SetUniform2f(const std::string& uniform_name, float v0, float v1) {
    VALIDATE_GL_(glUniform2f(GetUniformLocation(uniform_name), v0, v1));
  }

  void SetUniform3f(const std::string& uniform_name, float v0, float v1, float v2) {
    VALIDATE_GL_(glUniform3f(GetUniformLocation(uniform_name), v0, v1, v2));
  }

  void SetUniform4f(const std::string& uniform_name, float v0, float v1, float v2, float v3) {
    VALIDATE_GL_(glUniform4f(GetUniformLocation(uniform_name), v0, v1, v2, v3));
  }


  void SetUniformMat4f(const std::string& uniform_name, const glm::mat4& matrix) {
    VALIDATE_GL_(glUniformMatrix4fv(GetUniformLocation(uniform_name), 1, GL_FALSE, &matrix[0][0]));
  } 
  
  void SetUniformVec3f(const std::string& uniform_name, const glm::vec3& vec3) {
    VALIDATE_GL_(glUniform3f(GetUniformLocation(uniform_name), vec3.x, vec3.y, vec3.z ));
  }


private:

  int GetUniformLocation(const std::string name) {
    if (uniform_location_cache.find(name) != uniform_location_cache.end()) {
      return uniform_location_cache[name];
    }
    
    int uniform_location{};
    VALIDATE_GL_(uniform_location = glGetUniformLocation(_id, name.c_str()));
    if (uniform_location == -1) {
      std::cout << "Uniform " << name << " does not exist \n";
      assert(0);
    }
    else {
      uniform_location_cache[name] = uniform_location;
    }

    return uniform_location;
  }
};

class Texture {
private:
  unsigned int _id;
  std::string filepath;
  std::int32_t width;
  std::int32_t height;
  std::int32_t bits_per_pixel;
  unsigned char* local_buffer;

public:

  Texture(const std::string& path) 
    : _id(0), filepath(path), width(0), height(0), bits_per_pixel(0),  local_buffer(nullptr)
  {

    stbi_set_flip_vertically_on_load(1);
    local_buffer = stbi_load(path.c_str(), &width, &height, &bits_per_pixel, 4);

    VALIDATE_GL_(glGenTextures(1, &_id));
    VALIDATE_GL_(glBindTexture(GL_TEXTURE_2D, _id));

    VALIDATE_GL_(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    VALIDATE_GL_(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    VALIDATE_GL_(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    VALIDATE_GL_(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    
    VALIDATE_GL_(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, local_buffer));
    
    VALIDATE_GL_(glBindTexture(GL_TEXTURE_2D, 0));
    
    if (local_buffer) {
      stbi_image_free(local_buffer);
    }

  }

  ~Texture() {
    VALIDATE_GL_(glDeleteTextures(1, &_id));
  }

  void Bind(unsigned int slot = 0) {	
    
    VALIDATE_GL_(glActiveTexture(GL_TEXTURE0 + slot));
    VALIDATE_GL_(glBindTexture(GL_TEXTURE_2D, _id));

  }

  void UnBind() {
    VALIDATE_GL_(glBindTexture(GL_TEXTURE_2D, 0));
  }

};

namespace ShaderInput {

  bool ReadFromFile(const std::string& path, std::string& destination_string) {
    std::ifstream shader_file(path);

    if (!shader_file) {
      std::cout << "Error Opening file: " << path << std::endl;
      return false;
    }

    std::string line;

    while (std::getline(shader_file, line)) {
      destination_string += line + '\n';
    }

    return true;
  }

}