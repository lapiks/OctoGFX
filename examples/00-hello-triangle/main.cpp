#include <iostream>
#include <cassert>
#include <vector>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "octogfx/octogfx.h"

int main(int, char**) {
  if (!glfwInit()) {
    std::cerr << "Could not initialize GLFW" << std::endl;
    return 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "Voxel Engine", NULL, NULL);
  if (!window) {
    std::cerr << "Could not open window" << std::endl;
    glfwTerminate();
    return 1;
  }

  ogfx::Context ctx;
  ogfx::InitInfo info = {
    .platformData = ogfx::PlatformData {
      glfwGetWin32Window(window)
    },
    .resolution = { 640, 480 }
  };

  if (!ctx.init(info)) {
    std::cerr << "Octo RendererContext initialization failed" << std::endl;
    glfwTerminate();
    return 1;
  }

  ogfx::RenderPipelineHandle pipe = ctx.newRenderPipeline({});

  while (!glfwWindowShouldClose(window)) {
    // Check whether the user clicked on the close button (and any other
    // mouse/key event, which we don't use so far)
    glfwPollEvents();

    ctx.commitFrame();
  }

  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}