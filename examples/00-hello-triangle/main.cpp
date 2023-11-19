#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "octogfx/octogfx.h"

namespace utils {
  std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate);

    if (!file.is_open()) {
      return {};
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
  }
}

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
    std::cerr << "OctoGFX RendererContext initialization failed" << std::endl;
    glfwTerminate();
    return 1;
  }

  std::vector<char> shaderData = utils::readFile("C:/Users/dheni/source/repos/OctoGFX/examples/shaders/triangle.wgsl"); // todo: manage shaders path

  ogfx::ShaderHandle shader = ctx.newShader({ reinterpret_cast<const uint8_t*>(shaderData.data()), shaderData.size() });
  ogfx::RenderPipelineHandle pipe = ctx.newRenderPipeline({ shader });

  while (!glfwWindowShouldClose(window)) {
    // Check whether the user clicked on the close button (and any other
    // mouse/key event, which we don't use so far)
    glfwPollEvents();

    ctx.beginDefaultPass();
    ctx.applyPipeline(pipe);
    ctx.draw();
    ctx.endPass();
    ctx.commitFrame();
  }

  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}