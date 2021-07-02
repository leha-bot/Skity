#include "test_common.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

namespace test {

extern "C" void framebuffer_size_callback(GLFWwindow* window, int width,
                                          int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow* init_glfw_window(uint32_t width, uint32_t height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // multisample
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window =
      glfwCreateWindow(width, height, "Raw path render", nullptr, nullptr);

  if (window == nullptr) {
    exit(-2);
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    exit(-3);
  }

  return window;
}

GLuint create_shader(const char* source, GLenum type) {
  GLuint shader = glCreateShader(type);

  GLint success;
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[1024];
    glGetShaderInfoLog(shader, 1204, nullptr, info_log);
    std::cerr << "shader compile error = " << info_log;
    exit(-4);
  }

  return shader;
}

GLuint create_shader_program(const char* vs_code, const char* fs_code) {
  GLuint program = glCreateProgram();
  GLint success;

  GLuint vs = create_shader(vs_code, GL_VERTEX_SHADER);
  GLuint fs = create_shader(fs_code, GL_FRAGMENT_SHADER);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    GLchar info_log[1024];
    glGetProgramInfoLog(program, 1024, nullptr, info_log);
    std::cerr << "program link error " << info_log << std::endl;
    exit(-5);
  }

  return program;
}

TestApp::~TestApp() {
  if (window_) {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
}

void TestApp::Start() {
  Init();
  RunLoop();
}

void TestApp::Init() {
  window_ = init_glfw_window(window_width_, window_height_);
  assert(window_);

  glfwMakeContextCurrent(window_);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  this->OnInit();
}

void TestApp::RunLoop() {
  while (!glfwWindowShouldClose(window_)) {
    this->OnDraw();
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

}  // namespace test