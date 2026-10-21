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

// for imgui integration with camera
#include <imgui.h>

//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static bool PrintGLErrors = true; // just a temporary solution, will look forward to encapsulate or find a better architecture

void DisableGLErrorPrinting(){
  PrintGLErrors = false;
}

void EnableGLErrorPrinting(){
  PrintGLErrors = true;
}


void clear_gl_errors() {
  while (glGetError() != GL_NO_ERROR) {}
}

void log_gl_errors(const char* expression, std::size_t line, const char* file_name) {

  int error_code = glGetError();
  bool loop_ran = false;

  while (error_code != GL_NO_ERROR) {
    if (PrintGLErrors)
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
  bool drag_style_camera = false;

  bool left_click_pressed = false;
  float sensitivity = 0.08f;
  bool ImGui_shared_mouse = false;
  bool ImGui_shared_keyboard = false;
  ImGuiIO& ImGui_io;

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
  : window(window), 
    ImGui_io(ImGui::GetIO())
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
    if (ImGui_shared_mouse && ImGui_io.WantCaptureMouse) return;
    if (drag_style_camera) {
      if (!left_click_pressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
      {
        firstmouse = true;
        left_click_pressed = true;
      }

      if (left_click_pressed && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
      {
        left_click_pressed = false;
      }

      if (!left_click_pressed) return;
    }
    
    if (firstmouse){
    lastX = xpos;
    lastY = ypos;
    firstmouse = false;
    return;
    }

    float xoffset;
    float yoffset;

    if (drag_style_camera) {
      xoffset = lastX - xpos ;
      yoffset = ypos - lastY;
    } else {
      xoffset = xpos - lastX;
      yoffset = lastY - ypos; // reversed: y ranges bottom to top
    }
    
    
    lastX = xpos;
    lastY = ypos;
    
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
    firstmouse = true;
    take_input = true;
  }
  
  void SetCameraDisabled(){
    firstmouse = false; // only set to false for understanding and clarity
    // is set to false by mouse_callback
    take_input = false;
  }

  void SetCursorVisible() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  void SetCursorInvisible(){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disabling cursor... not camera
  }

  void SetDragStyleCamera(){
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    drag_style_camera = true;
  }

  void SetMouseSharedWithImGui(){
    ImGui_shared_mouse = true;
  }

  void SetKeyboardSharedWithImGui(){
    ImGui_shared_keyboard = true;
  }

  glm::vec3 GetPosition(){
    return cameraPos;
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
    if (!take_input) return;
    if (ImGui_shared_keyboard && ImGui_io.WantCaptureKeyboard) return;
    const float cameraSpeed = deltaTime * 3.0f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    
    // SetCameraMode(glfw_window, false);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      cameraPos.y += cameraSpeed;
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      cameraPos.y -= cameraSpeed;
    
    
  }

  void CameraDisablingKey_control(){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
      SetCameraDisabled();
      SetCursorVisible();
    }
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

  void Unbind() const {
    VALIDATE_GL_(glBindVertexArray(0));
  }

  static void UnbindAll(){
    VALIDATE_GL_(glBindVertexArray(0));
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

  void set_data(unsigned int target_type_GLenum, std::size_t buffer_size_bytes, void* buffer_start, unsigned int usage_type_GLenum){
    VALIDATE_GL_(glBufferData(target_type_GLenum, buffer_size_bytes, buffer_start, usage_type_GLenum));
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
  
  void set_data(unsigned int target_type_GLenum, std::size_t buffer_size_bytes, void* buffer_start, unsigned int usage_type_GLenum){
    VALIDATE_GL_(glBufferData(target_type_GLenum, buffer_size_bytes, buffer_start, usage_type_GLenum));
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
  std::unordered_map<std::string, int> nonexistent_uniform_reported;
  bool m_ignore_nonexistent_uniform = false;

public:

  ShaderProgram() : _id(0){

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

  void SetIgnoreNonExistentUniform(bool true_or_false){
    m_ignore_nonexistent_uniform = true_or_false;
  }

private:

  int GetUniformLocation(const std::string name) {
    if (uniform_location_cache.find(name) != uniform_location_cache.end()) {
      return uniform_location_cache[name];
    }
    
    int uniform_location{};
    VALIDATE_GL_(uniform_location = glGetUniformLocation(_id, name.c_str()));
    if (uniform_location == -1) {
      
      if (!m_ignore_nonexistent_uniform){
        std::cout << "\n \n \n \n !!!!!!!!!!!! Uniform " << name << " does not exist \n \n \n \n";
        assert(0);
      } else {
        if (nonexistent_uniform_reported.find(name) == nonexistent_uniform_reported.end() ||
            nonexistent_uniform_reported[name] == false) {// not yet reported
          std::cout << "Ignored non existent uniform: " << name << std::endl;
          nonexistent_uniform_reported[name] = true;
        }
      }
    }
    else {
      uniform_location_cache[name] = uniform_location;
    }

    return uniform_location;
  }
};

class Texture {
private:
  //unsigned int _id;
  std::string filepath;
  //std::string type;
  std::int32_t width;
  std::int32_t height;
  std::int32_t bits_per_pixel;
  
  unsigned char* local_buffer;


public:
  unsigned int _id;
  std::string type;

  Texture()
  : _id(0), filepath(""), width(0), height(0), bits_per_pixel(0),  local_buffer(nullptr) 
  {}

  Texture(const std::string& path) 
    : _id(0), filepath(path), width(0), height(0), bits_per_pixel(0),  local_buffer(nullptr)
  {

    SetFile(path);
  }

  ~Texture() {
    VALIDATE_GL_(glDeleteTextures(1, &_id));
  }

  void SetFile(const std::string& path){
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

  void Bind(unsigned int slot = 0) {	
    
    VALIDATE_GL_(glActiveTexture(GL_TEXTURE0 + slot));
    VALIDATE_GL_(glBindTexture(GL_TEXTURE_2D, _id));

  }

  void UnBind() {
    VALIDATE_GL_(glBindTexture(GL_TEXTURE_2D, 0));
  }

  static void UnBindAll() {
    VALIDATE_GL_(glBindTexture(GL_TEXTURE_2D, 0));
  }

};

// making mesh abstractions

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoordinates;
};

// struct Texture {};// already made

class Mesh {
  VertexArray VAO;
  VertexBuffer VBO;
  ElementBuffer EBO;
  
  public:
  // mesh data
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  std::vector<Texture> m_textures;

  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
  : m_vertices(vertices), m_indices(indices), m_textures(textures) 
  {
    SetMesh();
  }

  void PrepareDraw(ShaderProgram& shader_program){
    unsigned int diffuse_count = 1;
    unsigned int specular_count = 1;

    for (std::size_t i{}; i < m_textures.size(); ++i){
      m_textures[i].Bind(i);

      std::string number;
      std::string name = m_textures[i].type;

      if (name == "texture_diffuse"){
        number = std::to_string(diffuse_count);
        diffuse_count++;
      } else if (name == "texture_specular"){
        number = std::to_string(specular_count);
        specular_count++;
      }
      std::string uniform_name(name + number);

      shader_program.SetUniform1i(uniform_name.c_str(), i);


    }

    Texture::UnBindAll();
  }

  void Draw(){
    VAO.bind();
    VALIDATE_GL_(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0));
    VAO.Unbind();
  }

  private:

  void SetMesh(){
    VAO.bind();
    
    VBO.bind(GL_ARRAY_BUFFER);
    VBO.set_data(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data() ,GL_STATIC_DRAW);

    EBO.bind();
    EBO.set_data(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

    VAO.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position));
    VAO.push_arrtib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal));
    VAO.push_arrtib_pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texcoordinates));

    VAO.enable(0);
    VAO.enable(1);
    VAO.enable(2);

    VALIDATE_GL_(glBindVertexArray(0)); // unbind the vao

  }

};

class Model {

  std::vector<Mesh> m_meshes;
  std::string directory;


  void loadModel(std::string path){
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
      std::cout << "ERROR::ASSIMP:: " << import.GetErrorString() << std::endl;
      return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
  }
  
  void processNode(aiNode* node, const aiScene* scene){
    for (std::size_t i{}; i < node->mNumMeshes; ++i){
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      m_meshes.push_back(processMesh(mesh, scene));
    }

    for (std::size_t i{}; i < node->mNumChildren; ++i){
      processNode(node->mChildren[i], scene);   
    }
  }
  
  Mesh processMesh(aiMesh* mesh, const aiScene* scene){
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (std::size_t i{}; i < mesh->mNumVertices; ++i){
      Vertex vertex;
      // process vertex positions, normals and texture coordinates
      //[...]
      glm::vec3 vec;

      vec.x = mesh->mVertices[i].x;
      vec.y = mesh->mVertices[i].y;
      vec.x = mesh->mVertices[i].z;
      
      vertex.position = vec;


      vec.x = mesh->mNormals[i].x;
      vec.y = mesh->mNormals[i].y;
      vec.x = mesh->mNormals[i].z;
      
      vertex.normal = vec;

      vec.x = mesh->mVertices[i].x;
      vec.y = mesh->mVertices[i].y;
      vec.x = mesh->mVertices[i].z;
      
      if (mesh->mTextureCoords[0]){
        glm::vec2 tvec;
        
        tvec.x = mesh->mTextureCoords[0][i].x;
        tvec.y = mesh->mTextureCoords[0][i].y;
        vertex.texcoordinates = tvec;
      } else {
        vertex.texcoordinates = glm::vec2(0.0f);
      }

      vertices.push_back(vertex);
    }
    // process indices
    // [...]
    for (std::size_t i{}; i < mesh->mNumFaces; ++i){
      aiFace face = mesh->mFaces[i];
      for (std::size_t j{}; j < face.mNumIndices; ++j){
        indices.push_back(face.mIndices[j]);
      }
    }

    // process material

    if (mesh->mMaterialIndex >= 0){
      // [...]

      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      
      std::vector<Texture> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
      
      std::vector<Texture> specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

    }


    return Mesh(vertices, indices, textures);

  }

  std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType texture_type, std::string type_name){

    std::vector<Texture> textures;
    for (std::size_t i{}; i < material->GetTextureCount(texture_type); ++i){
      aiString str;
      material->GetTexture(texture_type, i, &str);
      Texture texture(directory + "/" + str.C_Str());
      texture.type = type_name;
      textures.push_back(texture);
    }
    return textures;

  }


  public:

  Model(std::string path){
    loadModel(path);
  }

  void Draw(ShaderProgram& shader_program){
    for (std::size_t i{}; i < m_meshes.size(); ++i){
      m_meshes[i].PrepareDraw(shader_program);
      m_meshes[i].Draw();
    }
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