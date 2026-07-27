#include <cstddef>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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


}