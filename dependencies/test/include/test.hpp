#pragma once
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <imgui.h>
#include <tuple>
#include <utility>
#include <optional>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "../../../src/Renderer/include/Renderer.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace test {

  class TestClearColor {
  public:
    static constexpr std::string_view NameString{ "Clear Color" };

  private:



  public:
    float color[3];

    TestClearColor(float red, float green, float blue)
      : color{ red, green, blue }
    {

    }

    TestClearColor(GLFWwindow* window)
      : color{ 0.5f, 0.5f, 0.5f }
    {
      std::cout << "TestClearColor constructed \n";
    }

    ~TestClearColor() {
      std::cout << "TestClearColor Destructed \n";
    }

    //void processInput() {}

    void OnOpenGLRender() {
      VALIDATE_GL_(glClearColor(color[0], color[1], color[2], 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    }

    void OnImGuiRender() {
      ImGui::ColorEdit3("Clear Color", color);
    }



  };

  class TestMultipleTexture {
  public:
    static constexpr std::string_view NameString{ "Multiple Textures" };
  private:
    int window_width{};
    int window_height{};
    
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

    TestMultipleTexture(GLFWwindow* window)
      : Hello_Triangle_Vertices{ // x,y,z,Tx,Ty,r,g,b
     -0.5f, -0.5f, 0.0f,        0.0f, 0.0f,            1.0f, 0.0f, 0.0f, // 0
      0.5f, -0.5f, 0.0f,        1.0f, 0.0f,            0.0f, 1.0f, 0.0f, // 1
      0.5f,  0.5f, 0.0f,        1.0f, 1.0f,            0.0f, 0.0f, 1.0f, // 2
     -0.5f,  0.5f, 0.0f,        0.0f, 1.0f,            0.0f, 1.0f, 0.0f // 3
      },

      index_buffer{
      0, 1, 2, 2, 3, 0
      },

      //proj(glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f)),

      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader),
      texture("Assets/hellotexture.png")

    {
      std::cout << "TestMultipleTexture constructed \n";
      
      glfwGetWindowSize(window, &window_width, &window_height);

      float unit_far{ 100.f }; // how much pixels for one far(left right bottom up) unit
      proj = glm::ortho(
        (0.f - (static_cast<float>(window_width) / 2.f)) / unit_far, // left
        (0.f + (static_cast<float>(window_width) / 2.f)) / unit_far, // right

        (0.f - (static_cast<float>(window_height) / 2.f)) / unit_far, // bottom
        (0.f + (static_cast<float>(window_height) / 2.f)) / unit_far // up
      );
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

    ~TestMultipleTexture() {
      std::cout << "TestMultipleTexture Deconstructed \n";
    }

    void processInput() {}

    void OnOpenGLRender() {
      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

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
  public:
    static constexpr std::string_view NameString{ "Rectangle" };
  private:
    int window_width{};
    int window_height{};

    std::array<float, 24> Hello_Triangle_Vertices = { // x,y,z,r,g,b
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 0
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 1
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // 2
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 3
    };

    std::array<unsigned int, 6> index_buffer = {
      0, 1, 2, 2, 3, 0
    };

    glm::mat4 proj;

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

    TestRectangle(GLFWwindow* window)
      : VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/fragmentshader.shader", FragmentShaderSource)),

      v_src(VertexShaderSource.c_str()),
      f_src(FragmentShaderSource.c_str()),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader)

    {
      std::cout << "TestRectangle constructed \n";
      //std::cout << "Vertex shader: " << VertexShaderSource << std::endl;
      //std::cout << "Fragment shader: " << FragmentShaderSource << std::endl;

      glfwGetWindowSize(window, &window_width, &window_height);

      float unit_far{ 100.f }; // how much pixels for one far(left right bottom up) unit
      proj = glm::ortho(
        (0.f - (static_cast<float>(window_width) / 2.f)) / unit_far, // left
        (0.f + (static_cast<float>(window_width) / 2.f)) / unit_far, // right

        (0.f - (static_cast<float>(window_height) / 2.f)) / unit_far, // bottom
        (0.f + (static_cast<float>(window_height) / 2.f)) / unit_far // up
      );

      proj = glm::scale(proj, glm::vec3(3.f, 3.f, 3.f));
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

    ~TestRectangle() {
      std::cout << "TestRectangle Destructed \n";
    }

    void processInput() {}

    void OnOpenGLRender() {

      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


      shader_program.Bind();;
      v_arr.bind();
      shader_program.SetUniformMat4f("u_MVP", proj);
      VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


    }

    void OnImGuiRender() {}


  };

  class TestTriangle {
  public:
    static constexpr std::string_view NameString{ "Triangle" };

  private:

    int window_width{};
    int window_height{};

    std::array<float, 18> Hello_Triangle_Vertices = { // x,y,z,r,g,b
     0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 0
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 1
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // 2
    };

    std::array<unsigned int, 3> index_buffer = {
      0, 1, 2
    };

    glm::mat4 proj;

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

    TestTriangle(GLFWwindow* window)
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
      std::cout << "TestTriangle constructed \n";

      glfwGetWindowSize(window, &window_width, &window_height);

      float unit_far{ 100.f }; // how much pixels for one far(left right bottom up) unit
      proj = glm::ortho(
        (0.f - (static_cast<float>(window_width) / 2.f)) / unit_far, // left
        (0.f + (static_cast<float>(window_width) / 2.f)) / unit_far, // right

        (0.f - (static_cast<float>(window_height) / 2.f)) / unit_far, // bottom
        (0.f + (static_cast<float>(window_height) / 2.f)) / unit_far // up
      );

      proj = glm::scale(proj, glm::vec3(3.f, 3.f, 3.f));

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

    ~TestTriangle() {
      std::cout << "TestTriangle Destructed \n";
    }

    void processInput() {}

    void OnOpenGLRender() {

      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      shader_program.Bind();;
      v_arr.bind();
      shader_program.SetUniformMat4f("u_MVP", proj);
      VALIDATE_GL_(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));


    }

    void OnImGuiRender() {}


  };


  class Test3DCube {
  public:
    static constexpr std::string_view NameString{ "3D Cube" };
  private:
    int window_width{};
    int window_height{};
    
    std::array<float, 180> Hello_Triangle_Vertices

    {

      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f



    };

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

    Test3DCube(GLFWwindow* window)
      :

      index_buffer{
      0, 1, 2, 2, 3, 0
      },


      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader),
      texture("Assets/hellotexture.png")

    {
      //std::cout << "TestMultipleTexture constructed \n";
      std::cout << NameString << " Constructed \n";
      
      glfwGetWindowSize(window, &window_width, &window_height);
      proj = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

      //std::cout << "Vertex shader: " << VertexShaderSource << std::endl;
      //std::cout << "Fragment shader: " << FragmentShaderSource << std::endl;


      v_arr.bind();
      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Hello_Triangle_Vertices, GL_STATIC_DRAW);

      //e_buff.bind();
      //e_buff.set_data(GL_ELEMENT_ARRAY_BUFFER, index_buffer, GL_STATIC_DRAW);

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
      v_arr.push_arrtib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 3 * sizeof(float));
      v_arr.push_arrtib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 5 * sizeof(float));

      v_arr.enable(0);
      v_arr.enable(1);
      //v_arr.enable(2);



      VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
      VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

      shader_program.Bind();

      texture.Bind();
      shader_program.SetUniform1i("u_Texture", 0);


    }

    ~Test3DCube() {
      std::cout << NameString << " Deconstructed \n";
    }

    //void processInput() {}

    void OnOpenGLRender() {
      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


      {
        shader_program.Bind();

        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f),
          glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 mvp = proj * view * model;


        shader_program.SetUniformMat4f("u_MVP", mvp);

        v_arr.bind();

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }
    }

    void OnImGuiRender() {

    }



  };




  class TestMultiple3DCube {
  public:
    static constexpr std::string_view NameString{ "Multiple 3D Cube" };
  private:
    int window_width{};
    int window_height{};

    std::array<float, 180> Hello_Triangle_Vertices

    {

      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f



    };
    // translations
    glm::vec3 cubePositions[10] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),
      glm::vec3(-1.3f, 1.0f, -1.5f)
    };

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

    ShaderProgram shader_program;
    Texture texture;

    glm::vec3 cameraPos = glm::vec3(0, 0, 0);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);


  public:

    TestMultiple3DCube(GLFWwindow* window)
      :

      index_buffer{
      0, 1, 2, 2, 3, 0
      },


      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader),
      texture("Assets/hellotexture.png")

    {
      //std::cout << "TestMultipleTexture constructed \n";
      std::cout << NameString << " Constructed \n";

      glfwGetWindowSize(window, &window_width, &window_height);
      proj = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

      //std::cout << "Vertex shader: " << VertexShaderSource << std::endl;
      //std::cout << "Fragment shader: " << FragmentShaderSource << std::endl;


      v_arr.bind();
      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Hello_Triangle_Vertices, GL_STATIC_DRAW);

      //e_buff.bind();
      //e_buff.set_data(GL_ELEMENT_ARRAY_BUFFER, index_buffer, GL_STATIC_DRAW);

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
      v_arr.push_arrtib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 3 * sizeof(float));
      v_arr.push_arrtib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 5 * sizeof(float));

      v_arr.enable(0);
      v_arr.enable(1);
      //v_arr.enable(2);



      VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
      VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

      shader_program.Bind();

      texture.Bind();
      shader_program.SetUniform1i("u_Texture", 0);


    }

    ~TestMultiple3DCube() {
      std::cout << NameString << " Deconstructed \n";
    }

    void processInput() {}

    void OnOpenGLRender() {
      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));



      shader_program.Bind();

      v_arr.bind();

      for (unsigned int i = 0; i < 10; i++)
      {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);

        float angle = 20.0f * i;

        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.8f));

        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        const float radius = 20.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0),
          glm::vec3(0.0, 1.0, 0.0));

        glm::mat4 mvp = proj * view * model;


        shader_program.SetUniformMat4f("u_MVP", mvp);

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }


      VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));

    }

    void OnImGuiRender() {

    }



  };






  class TestWalkAround_Multiple3DCube {
  public:
    static constexpr std::string_view NameString{ "Walk Around Multiple 3D Cube" };
  private:

    GLFWwindow* glfw_window = nullptr;
    Camera camera;
    int window_width{};
    int window_height{};

    std::array<float, 180> Hello_Triangle_Vertices

    {

      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f



    };
    // translations
    glm::vec3 cubePositions[10] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),
      glm::vec3(-1.3f, 1.0f, -1.5f)
    };

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

    
    ShaderProgram shader_program;
    Texture texture;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float deltaTime{};
    float lastFrame{};

  public:

    TestWalkAround_Multiple3DCube(GLFWwindow* window)
      :
      glfw_window(window),
      camera(window),

      index_buffer{
      0, 1, 2, 2, 3, 0
      },

      

      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/Shaders/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader),
      texture("Assets/hellotexture.png")

    {
      //std::cout << "TestMultipleTexture constructed \n";
      std::cout << NameString << " Constructed \n";


      glfwGetWindowSize(window, &window_width, &window_height);
      proj = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

      //std::cout << "Vertex shader: " << VertexShaderSource << std::endl;
      //std::cout << "Fragment shader: " << FragmentShaderSource << std::endl;
      //SetCameraMode(glfw_window, true);

      camera.SetCameraEnabled();
      camera.SetCursorInvisible();
      camera.SetMouseSharedWithImGui();
      camera.SetKeyboardSharedWithImGui();


      v_arr.bind();
      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Hello_Triangle_Vertices, GL_STATIC_DRAW);

      //e_buff.bind();
      //e_buff.set_data(GL_ELEMENT_ARRAY_BUFFER, index_buffer, GL_STATIC_DRAW);

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
      v_arr.push_arrtib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 3 * sizeof(float));
      v_arr.push_arrtib_pointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 5 * sizeof(float));

      v_arr.enable(0);
      v_arr.enable(1);
      //v_arr.enable(2);



      VALIDATE_GL_(glDeleteShader(vertex_shader.id()));
      VALIDATE_GL_(glDeleteShader(fragment_shader.id()));

      shader_program.Bind();

      texture.Bind();
      shader_program.SetUniform1i("u_Texture", 0);


    }

    ~TestWalkAround_Multiple3DCube() {
      // SetCameraMode(glfw_window, false);
      camera.SetCameraDisabled();
      camera.SetCursorVisible();
      std::cout << NameString << " Deconstructed \n";
    }

    void processInput() {
      camera.predefined_generic_WASD_control();
      camera.CameraDisablingKey_control();
    }

    void OnOpenGLRender() {
      camera.OnOpenGLRender();

      VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));



      shader_program.Bind();

      v_arr.bind();

      for (unsigned int i = 0; i < 10; i++)
      {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);

        float angle = 20.0f * i;

        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.8f));

        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        
        glm::mat4 view;
        view = camera.GetViewMatrix();;

        glm::mat4 mvp = proj * view * model;


        shader_program.SetUniformMat4f("u_MVP", mvp);

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }


      //VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));

    }

    void OnImGuiRender() {
      ImGui::Text("Use WASD keys to move the camera...");
      ImGui::Text("Use SPACE and LCTRL keys to move the camera Up or Down...");
      ImGui::Text("Move mouse to rotate camera");
      ImGui::Spacing();
      ImGui::Text("Press ESC to disable camera and enable mouse");
    }

    


  };

  class TestLightingAmbient{
    Camera camera;
    int window_width{};
    int window_height{};

    public:
    static constexpr std::string_view NameString{ "Lighting Ambient" };
    std::array<float, 6*3*6> Vertices{
    // front
    -0.5f,  0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
     0.5f,  0.5f, 0.5f,
 
     0.5f,  0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
     0.5f, -0.5f, 0.5f,
 
    // back

    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
 
     0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,

 
    // left

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,


    //right

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
 
 
    //up

     -0.5f, 0.5f,  0.5f,
      0.5f, 0.5f, -0.5f,
     -0.5f, 0.5f, -0.5f,
 
     -0.5f, 0.5f,  0.5f,
      0.5f, 0.5f,  0.5f,
      0.5f, 0.5f, -0.5f,


    //down 


     -0.5f, -0.5f,  0.5f,
      0.5f, -0.5f, -0.5f,
     -0.5f, -0.5f, -0.5f,
 
     -0.5f, -0.5f,  0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f, -0.5f, -0.5f,


    };
    glm::mat4 proj;
    glm::mat4 model_cube = glm::mat4(1);
    glm::mat4 model_light = glm::mat4(1);
    glm::vec3 cube_color = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 light_color = glm::vec3(1.f, 1.f, 1.f);

    glm::vec3 light_source_translation = glm::vec3(1.5f, 0.8f, -3.f);
    std::string VertexShaderSource;
    std::string FragmentShaderSource;

    bool VertexShaderSource_read_status;
    bool FragmentShaderSource_read_status;

    const char* v_src = VertexShaderSource.c_str();
    const char* f_src = FragmentShaderSource.c_str();

    ShaderVertex vertex_shader;
    ShaderFragment fragment_shader;
    ShaderProgram shader_program;

    VertexArray v_arr;
    VertexBuffer v_buff;
    public:

    TestLightingAmbient(GLFWwindow* window)
    : 
      
      camera(window),
      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLightingAmbient/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLightingAmbient/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader)
    {
      std::cout << NameString << " Constructed \n";
      
      glfwGetWindowSize(window, &window_width, &window_height);
      proj = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

      if (!VertexShaderSource_read_status) std::cout << "Failed to read Vertex Shader\n";
      if (!FragmentShaderSource_read_status) std::cout << "Failed to read Fragment Shader\n";

      v_arr.bind();
      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Vertices, GL_STATIC_DRAW);

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

      v_arr.enable(0);
      shader_program.Bind();

      camera.SetCameraEnabled();
      camera.SetDragStyleCamera();
      camera.SetMouseSharedWithImGui();
      camera.SetKeyboardSharedWithImGui();



      model_light = glm::translate(model_light, light_source_translation);
      model_light = glm::scale(model_light, glm::vec3(0.5f, 0.5f, 0.5f));
    }

    void OnOpenGLRender(){
      camera.OnOpenGLRender();

      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      glm::mat4 view = camera.GetViewMatrix();

      { // cube
        shader_program.Bind();
        v_arr.bind();

        glm::mat4 mvp = proj * view * model_cube;
        shader_program.SetUniform1i("rendering_light_source", 0);
        shader_program.SetUniformMat4f("u_MVP", mvp);
        shader_program.SetUniformVec3f("object_color", glm::vec3(1.0f, 0.5f, 0.5f));
        shader_program.SetUniformVec3f("light_color", light_color);


        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }


      { // light
        shader_program.Bind();
        v_arr.bind();
        
        
        glm::mat4 mvp = proj * view * model_light;

        shader_program.SetUniform1i("rendering_light_source", 1);
        shader_program.SetUniformMat4f("u_MVP", mvp);
        shader_program.SetUniformVec3f("light_color", glm::vec3(1.0f, 1.0f, 1.0f));

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }
      
    }

    void processInput(){
      camera.predefined_generic_WASD_control();
    }
    

    ~TestLightingAmbient(){
      std::cout << NameString << " Deconstructed" << std::endl;
      camera.SetCameraDisabled();
    }

    void OnImGuiRender() {
      ImGui::Text("Use WASD keys to move the camera...");
      ImGui::Text("Use SPACE and LCTRL keys to move the camera Up or Down...");
      ImGui::Text("Move mouse to rotate camera");
      ImGui::Spacing();
      ImGui::Text("Press ESC to disable camera and enable mouse");
    }
  };


  class TestLightingDiffuse{
    Camera camera;
    int window_width{};
    int window_height{};

    public:
    static constexpr std::string_view NameString{ "Lighting Diffuse" };
    std::array<float, 6*6*6> Vertices{ // xyz, normals
    // front
    -0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
     0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
 
     0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
     0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
 
    // back

    -0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
    -0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,
     0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
 
     0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
    -0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,
     0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,

 
    // left

    -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,
    -0.5f,  0.5f, -0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,

    -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f,  0.5f,  -1.f, 0.f, 0.f,


    //right

     0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,
     0.5f,  0.5f, -0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f,

     0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f,  0.5f,  1.f, 0.f, 0.f,
 
 
    //up

     -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,
     -0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,
 
     -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,


    //down 


     -0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,
     -0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,
 
     -0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,


    };
    glm::mat4 proj;
    glm::mat4 model_cube = glm::mat4(1);
    glm::mat4 model_light = glm::mat4(1);
    glm::vec3 cube_color = glm::vec3(0.2f, 0.8f, 0.2f); // green-ish
    //glm::vec3 light_color = glm::vec3(1.f, 0.3f, 0.3f); // red-ish
    glm::vec3 light_color = glm::vec3(1.f, 1.0f, 1.0f); 

    glm::vec3 light_source_translation = glm::vec3(1.5f, 0.8f, -3.f);
    std::string VertexShaderSource;
    std::string FragmentShaderSource;

    bool VertexShaderSource_read_status;
    bool FragmentShaderSource_read_status;

    const char* v_src = VertexShaderSource.c_str();
    const char* f_src = FragmentShaderSource.c_str();

    ShaderVertex vertex_shader;
    ShaderFragment fragment_shader;
    ShaderProgram shader_program;

    VertexArray v_arr;
    VertexBuffer v_buff;
    public:

    TestLightingDiffuse(GLFWwindow* window)
    : 
      
      camera(window),
      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLightingDiffuse/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLightingDiffuse/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader)
    {
      std::cout << NameString << " Constructed \n";
      
      glfwGetWindowSize(window, &window_width, &window_height);
      proj = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

      if (!VertexShaderSource_read_status) std::cout << "Failed to read Vertex Shader\n";
      if (!FragmentShaderSource_read_status) std::cout << "Failed to read Fragment Shader\n";

      v_arr.bind();
      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Vertices, GL_STATIC_DRAW);
      

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
      v_arr.push_arrtib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 3 * sizeof(float));


      v_arr.enable(0);
      v_arr.enable(1);

      shader_program.Bind();

      camera.SetCameraEnabled();
      camera.SetDragStyleCamera();
      camera.SetCursorVisible(); // we want the cursor to be visible for a drag style camera
      // allow proper keyboard and mouse sharing with imgui
      camera.SetMouseSharedWithImGui();
      camera.SetKeyboardSharedWithImGui();


      model_light = glm::translate(model_light, light_source_translation);
      model_light = glm::scale(model_light, glm::vec3(0.5f, 0.5f, 0.5f));
    }

    void OnOpenGLRender(){
      camera.OnOpenGLRender();

      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      glm::mat4 view = camera.GetViewMatrix();

      { // cube
        shader_program.Bind();
        v_arr.bind();

        glm::mat4 mvp = proj * view * model_cube;
        shader_program.SetUniform1i("rendering_light_source", 0);

        shader_program.SetUniformMat4f("projection", proj);
        shader_program.SetUniformMat4f("view", view);
        shader_program.SetUniformMat4f("model", model_cube);

        shader_program.SetUniformVec3f("object_color", cube_color);
        shader_program.SetUniformVec3f("light_color", light_color);
        shader_program.SetUniformVec3f("lightPos", light_source_translation);

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }


      { // light
        shader_program.Bind();
        v_arr.bind();
        
        
        glm::mat4 mvp = proj * view * model_light;

        shader_program.SetUniform1i("rendering_light_source", 1);
        
        shader_program.SetUniformMat4f("projection", proj);
        shader_program.SetUniformMat4f("view", view);
        shader_program.SetUniformMat4f("model", model_light);
        shader_program.SetUniformVec3f("lightPos", light_source_translation);


        shader_program.SetUniformVec3f("light_color", light_color);

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }
      
    }

    void processInput(){
      camera.predefined_generic_WASD_control();
    }
    

    ~TestLightingDiffuse(){
      std::cout << NameString << " Deconstructed" << std::endl;
      camera.SetCameraDisabled();
    }

    void OnImGuiRender() {
      ImGui::Text("Use WASD keys to move the camera...");
      ImGui::Text("Use SPACE and LCTRL keys to move the camera Up or Down...");
      ImGui::Text("Move mouse to rotate camera");
      ImGui::Spacing();
      ImGui::Text("Press ESC to disable camera and enable mouse");
      ImGui::Spacing();
      ImGui::ColorEdit3("Light Color : ", glm::value_ptr(light_color));
      ImGui::Spacing();
      ImGui::ColorEdit3("Cube Color : ", glm::value_ptr(cube_color));


    }
  };



  class TestLightingSpecular{
    Camera camera;
    int window_width{};
    int window_height{};

    public:
    static constexpr std::string_view NameString{ "Lighting Specular" };
    std::array<float, 6*6*6> Vertices{ // xyz, normals
    // front
    -0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
     0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
 
     0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
     0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
 
    // back

    -0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
    -0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,
     0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
 
     0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
    -0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,
     0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,

 
    // left

    -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,
    -0.5f,  0.5f, -0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,

    -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f,  0.5f,  -1.f, 0.f, 0.f,


    //right

     0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,
     0.5f,  0.5f, -0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f,

     0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f,  0.5f,  1.f, 0.f, 0.f,
 
 
    //up

     -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,
     -0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,
 
     -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,


    //down 


     -0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,
     -0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,
 
     -0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,


    };
    glm::mat4 proj;
    glm::mat4 model_cube = glm::mat4(1);
    glm::mat4 model_light = glm::mat4(1);
    glm::vec3 cube_color = glm::vec3(0.2f, 0.8f, 0.2f); // green-ish
    //glm::vec3 light_color = glm::vec3(1.f, 0.3f, 0.3f); // red-ish
    glm::vec3 light_color = glm::vec3(1.f, 1.0f, 1.0f); 

    glm::vec3 light_source_translation = glm::vec3(1.5f, 0.8f, -3.f);
    std::string VertexShaderSource;
    std::string FragmentShaderSource;

    bool VertexShaderSource_read_status;
    bool FragmentShaderSource_read_status;

    const char* v_src = VertexShaderSource.c_str();
    const char* f_src = FragmentShaderSource.c_str();

    ShaderVertex vertex_shader;
    ShaderFragment fragment_shader;
    ShaderProgram shader_program;

    VertexArray v_arr;
    VertexBuffer v_buff;
    public:

    TestLightingSpecular(GLFWwindow* window)
    : 
      
      camera(window),
      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLightingSpecular/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLightingSpecular/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader)
    {
      std::cout << NameString << " Constructed \n";
      
      glfwGetWindowSize(window, &window_width, &window_height);
      proj = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

      if (!VertexShaderSource_read_status) std::cout << "Failed to read Vertex Shader\n";
      if (!FragmentShaderSource_read_status) std::cout << "Failed to read Fragment Shader\n";

      v_arr.bind();
      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Vertices, GL_STATIC_DRAW);
      

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
      v_arr.push_arrtib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 3 * sizeof(float));


      v_arr.enable(0);
      v_arr.enable(1);

      shader_program.Bind();

      camera.SetCameraEnabled();
      camera.SetDragStyleCamera();
      camera.SetCursorVisible(); // we want the cursor to be visible for a drag style camera
      // allow proper keyboard and mouse sharing with imgui
      camera.SetMouseSharedWithImGui();
      camera.SetKeyboardSharedWithImGui();


      model_light = glm::translate(model_light, light_source_translation);
      model_light = glm::scale(model_light, glm::vec3(0.5f, 0.5f, 0.5f));
    }

    void OnOpenGLRender(){
      camera.OnOpenGLRender();

      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      glm::mat4 view = camera.GetViewMatrix();

      { // cube
        shader_program.Bind();
        v_arr.bind();

        glm::mat4 mvp = proj * view * model_cube;
        shader_program.SetUniform1i("rendering_light_source", 0);

        shader_program.SetUniformMat4f("projection", proj);
        shader_program.SetUniformMat4f("view", view);
        shader_program.SetUniformMat4f("model", model_cube);

        shader_program.SetUniformVec3f("object_color", cube_color);
        shader_program.SetUniformVec3f("light_color", light_color);
        shader_program.SetUniformVec3f("lightPos", light_source_translation);
        shader_program.SetUniformVec3f("viewPos", camera.GetPosition());

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }


      { // light
        shader_program.Bind();
        v_arr.bind();
        
        
        glm::mat4 mvp = proj * view * model_light;

        shader_program.SetUniform1i("rendering_light_source", 1);
        
        shader_program.SetUniformMat4f("projection", proj);
        shader_program.SetUniformMat4f("view", view);
        shader_program.SetUniformMat4f("model", model_light);
        shader_program.SetUniformVec3f("lightPos", light_source_translation);


        shader_program.SetUniformVec3f("light_color", light_color);

        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }
      
    }

    void processInput(){
      camera.predefined_generic_WASD_control();
    }
    

    ~TestLightingSpecular(){
      std::cout << NameString << " Deconstructed" << std::endl;
      camera.SetCameraDisabled();
    }

    void OnImGuiRender() {
      ImGui::Text("Use WASD keys to move the camera...");
      ImGui::Text("Use SPACE and LCTRL keys to move the camera Up or Down...");
      ImGui::Text("Move mouse to rotate camera");
      ImGui::Spacing();
      ImGui::Text("Press ESC to disable camera and enable mouse");
      ImGui::Spacing();
      ImGui::ColorEdit3("Light Color : ", glm::value_ptr(light_color));
      ImGui::Spacing();
      ImGui::ColorEdit3("Cube Color : ", glm::value_ptr(cube_color));


    }
  };

  class TestLighting_Materials{
    Camera camera;
    int window_width{};
    int window_height{};


    struct Material {
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;
      glm::vec3 pos;
      glm::vec3 color;
      glm::mat4 mat_model;
      float specular_power;
      enum class Type {
        Emerald, Pearl, Bronze, Gold, Cyan_Plastic, 
        Red_Plastic, Green_Rubber, Yellow_Rubber, None
      };
      Material() : mat_model(0.f){}
      Material(Type type)
      : ambient(0.f), diffuse(1.f), specular(0.f), mat_model(1.f), pos(0.f)
      {
        switch (type){

          case Type::Emerald:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.5); // less = less shine brightness, more = more shine brightness
            specular_power = 64.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.f, 1.f, 0.f); // color of the object;
          break;

          case Type::Pearl:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.5); // less = less shine brightness, more = more shine brightness
            specular_power = 2.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9f, 0.9f, 0.9f); // color of the object;
          break;

          case Type::Bronze:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.7); // less = less shine brightness, more = more shine brightness
            specular_power = 32.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.722f, 0.337f, 0.145f); // color of the object;
          break;

          case Type::Gold:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.9); // less = less shine brightness, more = more shine brightness
            specular_power = 64.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9804f, 0.8824f, 0.0078f); // color of the object;
          break;

          case Type::Cyan_Plastic:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 128.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.0f, 0.8f, 0.8f); // color of the object;
          break;

          case Type::Red_Plastic:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 256.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9f, 0.0f, 0.0f); // color of the object;
          break;

          case Type::Green_Rubber:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 128.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.0f, 0.9f, 0.0f); // color of the object;
          break;

          case Type::Yellow_Rubber:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 128.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9f, 0.9f, 0.0f); // color of the object;
          break;

          case Type::None:
            mat_model = glm::mat4(0.f);
          break;
        }  
      }
    };
    Material::Type Current_Material;
    struct Light{
      glm::vec3 color;
      glm::mat4 mat_model;
      glm::vec3 scale;
      glm::vec3 pos;
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;
      Light()
      : 
      color(1.0f, 1.0f, 1.0f), 
      mat_model(1), 
      scale(0.5f, 0.5f, 0.5f),
      ambient(1.f), diffuse(1.f), specular(1.f)
      {

      }
    };
    

    public:
    static constexpr std::string_view NameString{ "Lighting Meterials" };
    std::array<float, 6*6*6> Vertices{ // xyz, normals
    // front
    -0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
     0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
 
     0.5f,  0.5f, 0.5f,  0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
     0.5f, -0.5f, 0.5f,  0.f, 0.f, 1.f,
 
    // back

    -0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
    -0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,
     0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
 
     0.5f,  0.5f, -0.5f,  0.f, 0.f, -1.f,
    -0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,
     0.5f, -0.5f, -0.5f,  0.f, 0.f, -1.f,

 
    // left

    -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,
    -0.5f,  0.5f, -0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,

    -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,
    -0.5f, -0.5f,  0.5f,  -1.f, 0.f, 0.f,


    //right

     0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,
     0.5f,  0.5f, -0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f,

     0.5f,  0.5f,  0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f, -0.5f,  1.f, 0.f, 0.f,
     0.5f, -0.5f,  0.5f,  1.f, 0.f, 0.f,
 
 
    //up

     -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,
     -0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,
 
     -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,
      0.5f, 0.5f, -0.5f,  0.f, 1.f, 0.f,


    //down 


     -0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,
     -0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,
 
     -0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f,  0.5f,  0.f, -1.f, 0.f,
      0.5f, -0.5f, -0.5f,  0.f, -1.f, 0.f,


    };
    glm::mat4 proj;
    
    Material cube;
    Light light;

    glm::vec3 light_source_translation = glm::vec3(1.5f, 0.8f, -3.f);
    std::string VertexShaderSource;
    std::string FragmentShaderSource;

    bool VertexShaderSource_read_status;
    bool FragmentShaderSource_read_status;

    const char* v_src = VertexShaderSource.c_str();
    const char* f_src = FragmentShaderSource.c_str();

    ShaderVertex vertex_shader;
    ShaderFragment fragment_shader;
    ShaderProgram shader_program;

    VertexArray v_arr;
    VertexBuffer v_buff;
    public:

    TestLighting_Materials(GLFWwindow* window)
    : 
      
      camera(window),
      VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLighting_Materials/vertexshader.shader", VertexShaderSource)),
      FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLighting_Materials/fragmentshader.shader", FragmentShaderSource)),

      vertex_shader(&v_src),
      fragment_shader(&f_src),

      shader_program(vertex_shader, fragment_shader)
    {
      std::cout << NameString << " Constructed \n";
      
      camera.cameraPos = glm::vec3(-2.55f, -1.30f, -5.58f);
      camera.pitch = 12.9f; 
      camera.yaw = 50.83f; 

      glfwGetWindowSize(window, &window_width, &window_height);
      proj = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

      if (!VertexShaderSource_read_status) std::cout << "Failed to read Vertex Shader\n";
      if (!FragmentShaderSource_read_status) std::cout << "Failed to read Fragment Shader\n";

      light.pos = light_source_translation;
      light.mat_model = glm::translate(light.mat_model, light.pos);
      light.mat_model = glm::scale(light.mat_model, light.scale);

      Current_Material = Material::Type::None;

      v_arr.bind();
      v_buff.bind(GL_ARRAY_BUFFER);
      v_buff.set_data(GL_ARRAY_BUFFER, Vertices, GL_STATIC_DRAW);
      

      v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
      v_arr.push_arrtib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 3 * sizeof(float));


      v_arr.enable(0);
      v_arr.enable(1);

      shader_program.Bind();

      camera.SetCameraEnabled();
      camera.SetDragStyleCamera();
      camera.SetCursorVisible(); // we want the cursor to be visible for a drag style camera
      // allow proper keyboard and mouse sharing with imgui
      camera.SetMouseSharedWithImGui();
      camera.SetKeyboardSharedWithImGui();

    }

    void OnOpenGLRender(){
      camera.OnOpenGLRender();

      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      glm::mat4 view = camera.GetViewMatrix();

      { // cube
        shader_program.Bind();
        v_arr.bind();

        glm::mat4 mvp = proj * view * cube.mat_model;
        shader_program.SetUniform1i("rendering_light_source", 0);

        shader_program.SetUniformMat4f("projection", proj);
        shader_program.SetUniformMat4f("view", view);
        shader_program.SetUniformMat4f("model", cube.mat_model);

        shader_program.SetUniformVec3f("material.color", cube.color);
        shader_program.SetUniformVec3f("light.color", light.color);
        shader_program.SetUniformVec3f("light.pos", light.pos);
        shader_program.SetUniformVec3f("camera.pos", camera.GetPosition());

        shader_program.SetUniformVec3f("material.ambient", cube.ambient);
        shader_program.SetUniformVec3f("material.diffuse", cube.diffuse);
        shader_program.SetUniformVec3f("material.specular", cube.specular);
        shader_program.SetUniform1f("material.specular_power", cube.specular_power);


        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }


      { // light
        shader_program.Bind();
        v_arr.bind();
        

        shader_program.SetUniform1i("rendering_light_source", 1);
        
        shader_program.SetUniformMat4f("projection", proj);
        shader_program.SetUniformMat4f("view", view);
        shader_program.SetUniformMat4f("model", light.mat_model);
        
        shader_program.SetUniformVec3f("light.color", light.color);
        shader_program.SetUniformVec3f("light.ambient", light.ambient);
        shader_program.SetUniformVec3f("light.diffuse", light.diffuse);
        shader_program.SetUniformVec3f("light.specular", light.specular);


        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, 36));
      }
      
    }

    void processInput(){
      camera.predefined_generic_WASD_control();
    }
    

    ~TestLighting_Materials(){
      std::cout << NameString << " Deconstructed" << std::endl;
      camera.SetCameraDisabled();
    }

    void OnImGuiRender() {
      ImGui::Text("Use WASD keys to move the camera...");
      ImGui::Text("Use SPACE and LCTRL keys to move the camera Up or Down...");
      ImGui::Text("Move mouse to rotate camera");
      ImGui::Spacing();
      ImGui::Text("Press ESC to disable camera and enable mouse");
      ImGui::Spacing();
      ImGui::ColorEdit3("Light Color : ", glm::value_ptr(light.color));
      ImGui::Spacing();
      ImGui::ColorEdit3("Cube Color : ", glm::value_ptr(cube.color));

      //temp 
      ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
      ImGui::Text("Pitch: %.2f", camera.pitch);
      ImGui::Text("Yaw: %.2f", camera.yaw);
      ImGui::Spacing();
      ImGui::Text("Select desired Material:");
      
      if (Current_Material == Material::Type::None){
        if (ImGui::Button("Emerald")) {
          Current_Material = Material::Type::Emerald;
          cube = Material(Material::Type::Emerald);
        }
        if (ImGui::Button("Pearl")) {
          Current_Material = Material::Type::Pearl;
          cube = Material(Material::Type::Pearl);
        }
        if (ImGui::Button("Bronze")) {
          Current_Material = Material::Type::Bronze;
          cube = Material(Material::Type::Bronze);
        }
        if (ImGui::Button("Gold")) {
          Current_Material = Material::Type::Gold;
          cube = Material(Material::Type::Gold);
        }
        if (ImGui::Button("Cyan Plastic")) {
          Current_Material = Material::Type::Cyan_Plastic;
          cube = Material(Material::Type::Cyan_Plastic);
        }
        if (ImGui::Button("Red Plastic")) {
          Current_Material = Material::Type::Red_Plastic;
          cube = Material(Material::Type::Red_Plastic);
        }
        if (ImGui::Button("Green Rubber")) {
          Current_Material = Material::Type::Green_Rubber;
          cube = Material(Material::Type::Green_Rubber);
        }
        if (ImGui::Button("Yellow Rubber")) {
          Current_Material = Material::Type::Yellow_Rubber;
          cube = Material(Material::Type::Yellow_Rubber);
        }
        
      } else {
        if (ImGui::Button("<--- Select another Material")) { 
          Current_Material = Material::Type::None;
          cube = Material(Material::Type::None);
        }
      }


    }

    
  };

  class TestLighting_DiffuseMap {
    int m_window_width{};
    int m_window_height{};
    Camera m_camera;
    std::array<float, 6*6*(6+2)> m_Vertices{ //xyz, normals, Texture_xy
     -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
      0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,      1.0f, 0.0f,
      0.5f, 0.5f, -0.5f,      0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
      0.5f, 0.5f, -0.5f,      0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
     -0.5f, 0.5f, -0.5f,      0.0f, 0.0f, -1.0f,      0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
     
     -0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
      0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,       1.0f, 0.0f,
      0.5f, 0.5f,  0.5f,      0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
      0.5f, 0.5f,  0.5f,      0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
     -0.5f, 0.5f,  0.5f,      0.0f, 0.0f, 1.0f,       0.0f, 1.0f,
     -0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
    
     -0.5f, 0.5f, 0.5f,      -1.0f, 0.0f, 0.0f,       1.0f, 0.0f,
     -0.5f, 0.5f, -0.5f,     -1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
     -0.5f, -0.5f, 0.5f,     -1.0f, 0.0f, 0.0f,       0.0f, 0.0f,
     -0.5f, 0.5f, 0.5f,      -1.0f, 0.0f, 0.0f,       1.0f, 0.0f,

      0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f,
      0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
      0.5f, -0.5f, 0.5f,      1.0f, 0.0f, 0.0f,       0.0f, 0.0f,
      0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f,

     -0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,      0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,      1.0f, 1.0f,
      0.5f, -0.5f, 0.5f,      0.0f, -1.0f, 0.0f,      1.0f, 0.0f,
      0.5f, -0.5f, 0.5f,      0.0f, -1.0f, 0.0f,      1.0f, 0.0f,
     -0.5f, -0.5f, 0.5f,      0.0f, -1.0f, 0.0f,      0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,      0.0f, 1.0f,
    
     -0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,       0.0f, 1.0f,
      0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,       1.0f, 1.0f,
      0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
      0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
     -0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,       0.0f, 0.0f,
     -0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,       0.0f, 1.0f

    };
    
    struct Light{
      glm::vec3 color;
      glm::mat4 mat_model;
      glm::vec3 scale;
      glm::vec3 pos;
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;
      Light()
      : 
      color(1.0f, 1.0f, 1.0f), 
      mat_model(1), 
      scale(0.5f, 0.5f, 0.5f),
      ambient(1.f), diffuse(1.f), specular(1.f)
      {

      }
    };

    struct Material {
      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;
      glm::vec3 pos;
      glm::vec3 color;
      glm::mat4 mat_model;
      float specular_power;
      enum class Type {
        Emerald, Pearl, Bronze, Gold, Cyan_Plastic, 
        Red_Plastic, Green_Rubber, Yellow_Rubber, Neutral, None
      };
      Material() : mat_model(0.f){}
      Material(Type type)
      : ambient(0.f), diffuse(1.f), specular(0.f), mat_model(1.f), pos(0.f)
      {
        switch (type){

          case Type::Emerald:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.5); // less = less shine brightness, more = more shine brightness
            specular_power = 64.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.f, 1.f, 0.f); // color of the object;
          break;

          case Type::Pearl:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.5); // less = less shine brightness, more = more shine brightness
            specular_power = 2.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9f, 0.9f, 0.9f); // color of the object;
          break;

          case Type::Bronze:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.7); // less = less shine brightness, more = more shine brightness
            specular_power = 32.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.722f, 0.337f, 0.145f); // color of the object;
          break;

          case Type::Gold:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.9); // less = less shine brightness, more = more shine brightness
            specular_power = 64.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9804f, 0.8824f, 0.0078f); // color of the object;
          break;

          case Type::Cyan_Plastic:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 128.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.0f, 0.8f, 0.8f); // color of the object;
          break;

          case Type::Red_Plastic:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 256.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9f, 0.0f, 0.0f); // color of the object;
          break;

          case Type::Green_Rubber:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 128.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.0f, 0.9f, 0.0f); // color of the object;
          break;

          case Type::Yellow_Rubber:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 128.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9f, 0.9f, 0.0f); // color of the object;
          break;

          case Type::Neutral:
            ambient = glm::vec3(0.1); // less = dark, more = bright
            specular = glm::vec3(0.4); // less = less shine brightness, more = more shine brightness
            specular_power = 32.f; // area of shine, less = more area, more = less area,
            color = glm::vec3(0.9f, 0.9f, 9.0f); // color of the object;
          break;

          case Type::None:
            mat_model = glm::mat4(0.f);
          break;
        }  
      }
    };

    glm::mat4 m_proj;
    Material m_cube;
    Light m_light;

    glm::vec3 m_light_source_translation = glm::vec3(1.5f, 0.8f, -3.f);
    std::string m_VertexShaderSource;
    std::string m_FragmentShaderSource;

    bool m_VertexShaderSource_read_status;
    bool m_FragmentShaderSource_read_status;

    const char* m_v_src = m_VertexShaderSource.c_str();
    const char* m_f_src = m_FragmentShaderSource.c_str();

    ShaderVertex m_vertex_shader;
    ShaderFragment m_fragment_shader;
    ShaderProgram m_shader_program;

    VertexArray m_v_arr;
    VertexBuffer m_v_buff;

    public:
    static constexpr std::string_view NameString{"Diffuse Map"};
    
    TestLighting_DiffuseMap(GLFWwindow* window)
    : m_camera(window),
      
      m_VertexShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLighting_DiffuseMap/vertexshader.shader", m_VertexShaderSource)),
      m_FragmentShaderSource_read_status(ShaderInput::ReadFromFile("dependencies/test_Shaders/TestLighting_DiffuseMap/fragmentshader.shader", m_FragmentShaderSource)),
      
      m_vertex_shader(&m_v_src), 
      m_fragment_shader(&m_f_src),

      m_shader_program(m_vertex_shader, m_fragment_shader)

    {
      std::cout << NameString << " Constructed\n";

      glfwGetWindowSize(window, &m_window_width, &m_window_height);
      m_proj = glm::perspective(glm::radians(45.0f), static_cast<float>(m_window_width) / static_cast<float>(m_window_height), 0.1f, 100.0f);


      m_cube = Material(Material::Type::Neutral);
      m_light.pos = m_light_source_translation;
      m_light.scale = glm::vec3(0.5f);
      m_light.mat_model = glm::translate(m_light.mat_model, m_light.pos);
      m_light.mat_model = glm::scale(m_light.mat_model, m_light.scale);



      if (!m_VertexShaderSource_read_status) std::cout << "Failed to read Vertex Shader\n";
      if (!m_FragmentShaderSource_read_status) std::cout << "Failed to read Fragment Shader\n";

      m_v_arr.bind();
      m_v_buff.bind(GL_ARRAY_BUFFER);
      m_v_buff.set_data(GL_ARRAY_BUFFER, m_Vertices, GL_STATIC_DRAW);

      m_v_arr.push_arrtib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) *  8, 0);
      m_v_arr.push_arrtib_pointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) *  8, 3 * sizeof(float));
      m_v_arr.push_arrtib_pointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) *  8, 6 * sizeof(float));

      m_v_arr.enable(0);
      m_v_arr.enable(1);
      m_v_arr.enable(2);
      
      m_shader_program.Bind();
      
      m_camera.SetCameraEnabled();
      m_camera.SetDragStyleCamera();
      m_camera.SetCursorVisible(); // we want the cursor to be visible for a drag style camera
      // allow proper keyboard and mouse sharing with imgui
      m_camera.SetMouseSharedWithImGui();
      m_camera.SetKeyboardSharedWithImGui();

      m_camera.cameraPos = glm::vec3(-2.55f, -1.30f, -5.58f);
      m_camera.pitch = 12.9f; 
      m_camera.yaw = 50.83f; 

    }

    ~TestLighting_DiffuseMap(){
      std::cout << NameString << " Deconstructed\n";
    }

    void OnOpenGLRender(){
      m_camera.OnOpenGLRender();
      VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      glm::mat4 view = m_camera.GetViewMatrix();

      { //cube 

        m_shader_program.Bind();
        m_shader_program.SetUniform1f("rendering_light_source", 0);
        m_shader_program.SetUniformMat4f("model", m_cube.mat_model);
        m_shader_program.SetUniformMat4f("view", view);
        m_shader_program.SetUniformMat4f("projection", m_proj);
        m_shader_program.SetUniformVec3f("material.ambient", m_cube.ambient);
        m_shader_program.SetUniformVec3f("material.diffuse", m_cube.diffuse);
        m_shader_program.SetUniformVec3f("material.specular", m_cube.specular);
        m_shader_program.SetUniformVec3f("material.color", m_cube.color);
        m_shader_program.SetUniform1f("material.specular_power", m_cube.specular_power);

        m_shader_program.SetUniformVec3f("light.color", m_light.color);
        m_shader_program.SetUniformVec3f("light.pos", m_light.pos);

        m_shader_program.SetUniformVec3f("camera.pos", m_camera.GetPosition());
        
        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size()));

      }

      { // light 

        m_shader_program.Bind();
        m_shader_program.SetUniform1f("rendering_light_source", 1);
        m_shader_program.SetUniformMat4f("model", m_light.mat_model);
        m_shader_program.SetUniformMat4f("view", view);
        m_shader_program.SetUniformMat4f("projection", m_proj);
        //m_shader_program.SetUniformVec3f("material.ambient", m_cube.ambient);
        //m_shader_program.SetUniformVec3f("material.diffuse", m_cube.diffuse);
        //m_shader_program.SetUniformVec3f("material.specular", m_cube.specular);
        //m_shader_program.SetUniformVec3f("material.color", m_cube.color);
        //m_shader_program.SetUniform1f("material.specular_power", m_cube.specular_power);

        m_shader_program.SetUniformVec3f("light.color", m_light.color);
        m_shader_program.SetUniformVec3f("light.pos", m_light.pos);

        m_shader_program.SetUniformVec3f("camera.pos", m_camera.GetPosition());
        
        VALIDATE_GL_(glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size()));

      }

      




    }

    void processInput(){
      m_camera.predefined_generic_WASD_control();
    }

    void OnImGuiRender(){}

    
  };


  template <typename... Tests>
  class TestMenu {

    GLFWwindow* window = nullptr;

    static_assert(sizeof...(Tests) != 0, "Cannot create a test menu with zero tests");

    const std::size_t MenuHome{ std::numeric_limits<std::size_t>::max() };
    std::size_t CurrentTest{ MenuHome };

    std::tuple<std::optional<Tests>...> tests;


    template <std::size_t... indexes>
    void internal_OnOpenGLRender(std::index_sequence<indexes...>) {
      //((CurrentTest == indexes ? std::get<indexes>(tests)->OnOpenGLRender() : void()), ...);
      
      ((CurrentTest == indexes ? 
        [&]
        {
          if constexpr (requires { std::get<indexes>(tests)->OnOpenGLRender(); })
          {
            std::get<indexes>(tests)->OnOpenGLRender();
          } 
          else 
          {
            void();
          }
        }()
        
        : void()), ...
      );
    
    
    }

    template <std::size_t... indexes>
    void internal_OnImGuiRender(std::index_sequence<indexes...>) {
      //((CurrentTest == indexes ? std::get<indexes>(tests)->OnImGuiRender() : void()), ...);
      
      ((CurrentTest == indexes ? 
        [&]
        {
          if constexpr (requires { std::get<indexes>(tests)->OnImGuiRender(); })
          {
            std::get<indexes>(tests)->OnImGuiRender();
          } 
          else 
          {
            void();
          }
        }()
        
        : void()), ...
      );
    
    }

    template <std::size_t... indexes>
    void internal_processInput(std::index_sequence<indexes...>) {
      ((CurrentTest == indexes ? 
        [&]
        {
          if constexpr (requires { std::get<indexes>(tests)->processInput(); })
          {
            std::get<indexes>(tests)->processInput();
          } 
          else 
          {
            void();
          }
        }()
        
        : void()), ...
      );
    }

    template <std::size_t... indexes>
    void internal_MenuRender(std::index_sequence<indexes...>) {
      (
        (
          ImGui::Button(Tests::NameString.data())
          ? (CurrentTest = indexes, std::get<indexes>(tests).emplace(window), void())
          : void()
          ),
        ...
        );
    }

    void RenderMenu() {
      internal_MenuRender(std::index_sequence_for<Tests...>{});
    }

    template <std::size_t... indexes>
    void internal_current_obj_destruction(std::index_sequence<indexes...>) {
      (
        (
          CurrentTest == indexes
          ? (std::get<indexes>(tests).reset(), void())
          : void()
          ),
        ...
        );
    }


  public:
    TestMenu(GLFWwindow* window)
    : window(window)
    {
      //CurrentTest = 0;
    }

    void OnOpenGLRender() {
      if (CurrentTest != MenuHome) {
        internal_OnOpenGLRender(std::index_sequence_for<Tests...>{});
      }
      else {
        VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 1.f));
        VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      }
    }



    void OnImGuiRender() {
      if (CurrentTest != MenuHome) {
        if (ImGui::Button("< Back")) {


          internal_current_obj_destruction(std::index_sequence_for<Tests...>{});
          CurrentTest = MenuHome;
        }
        internal_OnImGuiRender(std::index_sequence_for<Tests...>{});
      }
      else {
        RenderMenu();
      }

    }


    void processInput() {
      if (CurrentTest != MenuHome) {
        internal_processInput(std::index_sequence_for<Tests...>{});
      }
      else {
        VALIDATE_GL_(glClearColor(0.1f, 0.1f, 0.1f, 1.f));
        VALIDATE_GL_(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

      }
    }

  };
}