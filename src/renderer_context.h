#pragma once

#include <webgpu/webgpu.h>
#include <vector>

#include "octogfx/octogfx.h"

constexpr uint32_t MAX_PASSES = 512;
constexpr uint32_t MAX_PIPELINES = 512;

namespace ogfx {
  struct InitInfo;
  struct RenderPipelineDesc;

  struct RenderPass {
    bool create(WGPUDevice device, const RenderPipelineDesc& desc);
    void destroy();

    WGPURenderPassEncoder m_renderPass;
  };

  struct RenderPipeline {
    bool create(WGPUDevice device, const RenderPipelineDesc& desc);
    void destroy();

    WGPURenderPipeline m_renderPipeline;
  };

  template<typename T>
  struct HandleAllocator {
    inline void allocate(T& handle) {
      handle.id = m_currentId++;
    }

  private:
    uint16_t m_currentId = 0;
  };

  struct RendererContext {
    bool init(const InitInfo& info);
    void shutdown();

    RenderPipelineHandle newRenderPipeline(const RenderPipelineDesc& desc);

    void applyPipeline(RenderPipelineHandle handle);
    void commitFrame();

  private:
    WGPUInstance m_instance;
    WGPUSurface m_surface;
    WGPUAdapter m_adapter;
    std::vector<WGPUFeatureName> m_features;
    WGPUDevice m_device;
    WGPUQueue m_queue;
    WGPUSwapChain m_swapChain;

    RenderPassHandle m_currentPass;

    RenderPipeline m_renderPipelines[MAX_PIPELINES];
    HandleAllocator<RenderPipelineHandle> m_renderPipelineAlloc;
    RenderPass m_renderPasses[MAX_PASSES];
    HandleAllocator<RenderPassHandle> m_renderPassAlloc;
  };
}