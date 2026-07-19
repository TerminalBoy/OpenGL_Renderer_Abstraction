#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include "Renderer/include/Renderer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../dependencies/mathrix/include/mathrix.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


 
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(1600, 900, "Learn OpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create a window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to oinitialize GLAD\n";
    return -1;
  }

  glViewport(0, 0, 1600, 900);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");


  std::array Hello_Triangle_Vertices = { // x,y,z,Tx,Ty,r,g,b
    -0.5f, -0.5f, 0.0f,        0.0f, 0.0f,            1.0f, 0.0f, 0.0f, // 0
     0.5f, -0.5f, 0.0f,        1.0f, 0.0f,            0.0f, 1.0f, 0.0f, // 1
     0.5f,  0.5f, 0.0f,        1.0f, 1.0f,            0.0f, 0.0f, 1.0f, // 2
    -0.5f,  0.5f, 0.0f,        0.0f, 1.0f,            0.0f, 1.0f, 0.0f // 3
  };

  std::array index_buffer = {
    0, 1, 2, 2, 3, 0
  };


  VertexArray v_arr;
  VertexBuffer v_buff;
  ElementBuffer e_buff;

  glm::mat4 proj = glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);
  //glm::mat4 proj(1.f);
  
  v_arr.bind();

  v_buff.bind(GL_ARRAY_BUFFER);
  v_buff.set_data(GL_ARRAY_BUFFER, Hello_Triangle_Vertices, GL_STATIC_DRAW);

  e_buff.bind();
  e_buff.set_data(GL_ELEMENT_ARRAY_BUFFER, index_buffer, GL_STATIC_DRAW);

  v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
  v_arr.push_arrtib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 3 * sizeof(float));
  v_arr.push_arrtib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 5 * sizeof(float));

  v_arr.enable(0);
  v_arr.enable(1);
  v_arr.enable(2);


  std::string VertexShaderSource;
  std::string FragmentShaderSource;

  ShaderInput::ReadFromFile("dependencies/Shaders/vertexshader.shader", VertexShaderSource);
  ShaderInput::ReadFromFile("dependencies/Shaders/fragmentshader.shader", FragmentShaderSource);

  const char* v_src = VertexShaderSource.c_str();
  const char* f_src = FragmentShaderSource.c_str();

  ShaderVertex vertex_shader(&v_src);
  ShaderFragment fragment_shader(&f_src);

  ShaderProgram shader_program(vertex_shader, fragment_shader);

  VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
  VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

  shader_program.Bind();
  Texture texture("Assets/hellotexture.png");
  texture.Bind();
  shader_program.SetUniform1i("u_Texture", 0);

  float camera_x{};
  float camera_y{};


  while (!glfwWindowShouldClose(window)) {



    processInput(window);

    VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
    VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT));


    //VALIDATE_GL_(glUseProgram(shader_program.id()));
    shader_program.Bind();
    
    
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(camera_x * 8.f, camera_y * 4.5f, 0.0f));

    glm::mat4 mvp = proj * view;

    
    shader_program.SetUniformMat4f("u_MVP", mvp);
    
    
    
    //VALIDATE_GL_(glBindVertexArray(v_arr.id()));
    v_arr.bind();

    VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");
    ImGui::SliderFloat("camera x", &camera_x, -1.f, 1.f);
    ImGui::SliderFloat("camera y", &camera_y, -1.f, 1.f);


    
    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}