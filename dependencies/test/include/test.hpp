#pragma once
#include <cstddef>
#include <iostream>
#include <imgui.h>
#include <tuple>
#include <utility>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Renderer/include/Renderer.hpp"

namespace test {

  class TestClearColor {
  public:
    float color[3];

    TestClearColor(float red, float green, float blue)
      : color{ red, green, blue }
    {

    }

    void ClearColor() {
      VALIDATE_GL_(glClearColor(color[0], color[1], color[2], 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT));
    }

    void OnImGuiRender() {
      ImGui::ColorEdit3("Clear Color", color);
    }
  };

  class TestMultipleTexture {

    std::array<float, 32> Hello_Triangle_Vertices;

    std::array<int, 6> index_buffer;

    glm::mat4 proj;

    std::string VertexShaderSource;
    std::string FragmentShaderSource;

    bool VertexShaderSource_read_status;
    bool FragmentShaderSource_read_status;

    const char* v_src = VertexShaderSource.c_str();
    const char* f_src = FragmentShaderSource.c_str();

    ShaderVertex vertex_shader;
    ShaderFragment fragment_shader;

    VertexArray v_arr;
    VertexBuffer v_buff;
    ElementBuffer e_buff;

    float cameraA_x{};
    float cameraA_y{};

    float cameraB_x{};
    float cameraB_y{};

    ShaderProgram shader_program;
    Texture texture;
  public:

    TestMultipleTexture()
      : Hello_Triangle_Vertices{ // x,y,z,Tx,Ty,r,g,b
     -0.5f, -0.5f, 0.0f,        0.0f, 0.0f,            1.0f, 0.0f, 0.0f, // 0
      0.5f, -0.5f, 0.0f,        1.0f, 0.0f,            0.0f, 1.0f, 0.0f, // 1
      0.5f,  0.5f, 0.0f,        1.0f, 1.0f,            0.0f, 0.0f, 1.0f, // 2
     -0.5f,  0.5f, 0.0f,        0.0f, 1.0f,            0.0f, 1.0f, 0.0f // 3
      },

      index_buffer{
      0, 1, 2, 2, 3, 0
      },

      proj(glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f)),

      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader),
      texture("Assets/hellotexture.png")

    {


      //std::cout << "Vertex shader: " << VertexShaderSource << std::endl;
      //std::cout << "Fragment shader: " << FragmentShaderSource << std::endl;


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



      VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
      VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

      shader_program.Bind();

      texture.Bind();
      shader_program.SetUniform1i("u_Texture", 0);


    }

    void OnOpenGLRender() {
      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT));

      {
        shader_program.Bind();


        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(cameraA_x * 8.f, cameraA_y * 4.5f, 0.0f));

        glm::mat4 mvp = proj * view;


        shader_program.SetUniformMat4f("u_MVP", mvp);

        v_arr.bind();

        VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
      }


      {
        shader_program.Bind();
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(cameraB_x * 8.f, cameraB_y * 4.5f, 0.0f));

        glm::mat4 mvp = proj * view;


        shader_program.SetUniformMat4f("u_MVP", mvp);
        v_arr.bind();

        VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

      }
    }

    void OnImGuiRender() {
      ImGui::SliderFloat("cameraA x", &cameraA_x, -1.f, 1.f);
      ImGui::SliderFloat("cameraA y", &cameraA_y, -1.f, 1.f);

      ImGui::Spacing();
      ImGui::Spacing();
      ImGui::Spacing();
      ImGui::Spacing();


      ImGui::SliderFloat("cameraB x", &cameraB_x, -1.f, 1.f);
      ImGui::SliderFloat("cameraB y", &cameraB_y, -1.f, 1.f);
    }
  };

  class TestRectangle {

    std::array<float, 24> Hello_Triangle_Vertices = { // x,y,z,r,g,b
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 0
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 1
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // 2
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 3
    };

    std::array<unsigned int, 6> index_buffer = {
      0, 1, 2, 2, 3, 0
    };


    VertexArray v_arr;
    VertexBuffer v_buff;
    ElementBuffer e_buff;


    std::string VertexShaderSource;
    std::string FragmentShaderSource;

    bool VertexShaderSource_read_status;
    bool FragmentShaderSource_read_status;

    const char* v_src;
    const char* f_src;

    ShaderVertex vertex_shader;
    ShaderFragment fragment_shader;

    ShaderProgram shader_program;

  public:

    TestRectangle()
      : VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/fragmentshader.shader", FragmentShaderSource)),

      v_src(VertexShaderSource.c_str()),
      f_src(FragmentShaderSource.c_str()),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader)

    {

      //std::cout << "Vertex shader: " << VertexShaderSource << std::endl;
      //std::cout << "Fragment shader: " << FragmentShaderSource << std::endl;


      v_arr.bind();

      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Hello_Triangle_Vertices, GL_STATIC_DRAW);

      e_buff.bind();
      e_buff.set_data(GL_ELEMENT_ARRAY_BUFFER, index_buffer, GL_STATIC_DRAW);

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
      v_arr.push_arrtib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 3 * sizeof(float));

      v_arr.enable(0);
      v_arr.enable(1);

      VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
      VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

    }

    void OnOpenGLRender() {

      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT));

      shader_program.Bind();;
      v_arr.bind();
      VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


    }

    void OnImGuiRender() {}

  };

  class TestTriangle {

    std::array<float, 18> Hello_Triangle_Vertices = { // x,y,z,r,g,b
     0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 0
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 1
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // 2
    };

    std::array<unsigned int, 3> index_buffer = {
      0, 1, 2
    };


    VertexArray v_arr;
    VertexBuffer v_buff;
    ElementBuffer e_buff;


    std::string VertexShaderSource;
    std::string FragmentShaderSource;

    bool VertexShaderSource_read_status;
    bool FragmentShaderSource_read_status;

    const char* v_src;
    const char* f_src;

    ShaderVertex vertex_shader;
    ShaderFragment fragment_shader;

    ShaderProgram shader_program;

  public:

    TestTriangle()
      : VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/fragmentshader.shader", FragmentShaderSource)),

      v_src(VertexShaderSource.c_str()),
      f_src(FragmentShaderSource.c_str()),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader)

    {

      //std::cout << "Vertex shader: " << VertexShaderSource << std::endl;
      //std::cout << "Fragment shader: " << FragmentShaderSource << std::endl;


      v_arr.bind();

      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Hello_Triangle_Vertices, GL_STATIC_DRAW);

      e_buff.bind();
      e_buff.set_data(GL_ELEMENT_ARRAY_BUFFER, index_buffer, GL_STATIC_DRAW);

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
      v_arr.push_arrtib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 3 * sizeof(float));

      v_arr.enable(0);
      v_arr.enable(1);

      VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
      VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

    }

    void OnOpenGLRender() {

      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT));
      shader_program.Bind();;
      v_arr.bind();
      VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


    }

    void OnImGuiRender() {}

  };

  template <typename... Tests>
  class TestMenu {

    static_assert(sizeof...(Tests) != 0, "Cannot create a test menu with zero tests");

    const std::size_t MenuHome{ std::numeric_limits<std::size_t>::max() };
    std::size_t CurrentTest{ MenuHome };

    std::tuple<Tests...> tests;


    template <std::size_t... indexes>
    void internal_OnOpenGLRender(std::index_sequence<indexes...>) {
      ((CurrentTest == indexes ? std::get<indexes>(tests).OnOpenGLRender() : void()), ...);
    }

    template <std::size_t... indexes>
    void internal_OnImGuiRender(std::index_sequence<indexes...>) {
      ((CurrentTest == indexes ? std::get<indexes>(tests).OnImGuiRender() : void()), ...);
    }
      
    public:
    TestMenu() 
    {
      CurrentTest = 0;
    }

    void OnOpenGLRender() {
      internal_OnOpenGLRender(std::index_sequence_for<Tests...>{});
    }
  
    void OnImGuiRender() {
      if (CurrentTest != MenuHome) {
        if (ImGui::Button("< Back")) CurrentTest = MenuHome;
        internal_OnImGuiRender(std::index_sequence_for<Tests...>{});
      }
      else {

      }
      
    }

  };
}