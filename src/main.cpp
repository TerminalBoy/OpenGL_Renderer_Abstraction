// float pitch{};
// float yaw{-90.f};
// float lastX = 800;
// float lastY = 450;
// bool firstmouse = false;

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
#include "../dependencies/test/include/test.hpp"

// gloabal temporarily, ill come up with a solution soon
 
int main() {
  { // code block so glfw clears its internal stack, and manage resources // absence of this causes segfaults
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

  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // capture mouse inputs, dont show the cursor


  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to oinitialize GLAD\n";
    return -1;
  }

  VALIDATE_GL_(glEnable(GL_DEPTH_TEST));

  glViewport(0, 0, 1600, 900);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


  //test::TestClearColor bg(0.1f, 0.1f, 0.1f);
  //test::TestMultipleTexture mt;
  //test::TestRectangle rt;
  //test::TestTriangle tr;
  
  glfwSetWindowUserPointer(window, nullptr); 

  test::TestMenu
    <
    test::TestMultipleTexture, 
    test::TestTriangle, 
    test::TestClearColor, 
    test::TestRectangle,
    test::Test3DCube,
    test::TestMultiple3DCube,
    test::TestWalkAround_Multiple3DCube,
    test::TestLightingScene
    > 
    menu(window);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");


  
  while (!glfwWindowShouldClose(window)) {



    processInput(window);
    
    menu.processInput();
    //bg.ClearColor();
    //mt.OnOpenGLRender();
    //rt.OnOpenGLRender();
    //tr.OnOpenGLRender();

    menu.OnOpenGLRender();


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");

    //bg.OnImGuiRender();
    //mt.OnImGuiRender();
    //rt.OnImGuiRender();
    //tr.OnImGuiRender();
    menu.OnImGuiRender();
    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  
  } // end of glfw code block
  glfwTerminate();
  return 0;
}