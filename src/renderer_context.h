#pragma once

#include <webgpu/webgpu.h>
#include <vector>

namespace ogfx {
  struct InitInfo;

  struct RendererContext {
    bool init(const InitInfo& info);
    void shutdown();

    void commitFrame();

  private:
    WGPUInstance m_instance;
    WGPUSurface m_surface;
    WGPUAdapter m_adapter;
    std::vector<WGPUFeatureName> m_features;
    WGPUDevice m_device;
    WGPUQueue m_queue;
    WGPUSwapChain m_swapChain;
  };
}