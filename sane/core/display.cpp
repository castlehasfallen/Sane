#include "sane/core/display.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "sane/events/inputs.hpp"
#include "sane/logging/log.hpp"

namespace
{
  void key_forwarder(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    Sane::Input::KeyHandler::Process(key, scancode, action, mods);
  }

  void cursor_forwarder(GLFWwindow* window, double xpos, double ypos)
  {
    Sane::Input::MouseHandler::Process(xpos, ypos);
  }

  void mouse_forwarder(GLFWwindow* window, int button, int action, int mods)
  {
    Sane::Input::MouseHandler::Process(button, action, mods);
  }

  static void error_callback(int error, const char* description) {
    SANE_WARN(description);
  }
}

namespace Sane
{
  Display::Display(const char* name, int32_t width, int32_t height)
  {
    if (!glfwInit())
      exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, name, NULL, NULL);
    if (!window_) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window_, key_forwarder);
    glfwSetCursorPosCallback(window_, cursor_forwarder);
    glfwSetMouseButtonCallback(window_, mouse_forwarder);

    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SANE_INFO("Created display");
  }

  Display::~Display() {
    glDeleteVertexArrays(1, &vao_);
    glfwDestroyWindow(window_);
    glfwTerminate();

    SANE_INFO("Destroyed display");
  }

  void Display::Update() {
    glfwSwapBuffers(window_);
    glfwPollEvents();

    int32_t width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  glm::mat4 Display::GetOrthoProjection() {
    float ratio;
    int32_t width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    ratio = width / (float)height;

    return glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
  }

  glm::mat4 Display::GetPersProjection() {
    float ratio;
    int32_t width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    ratio = width / (float)height;
    return glm::perspective(45.0f, ratio, 1.0f, 200.0f);
  }
}