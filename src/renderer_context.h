#pragma once

#include <webgpu/webgpu.h>
#include <vector>

#include "octogfx/octogfx.h"

constexpr uint32_t MAX_PASSES = 512;
constexpr uint32_t MAX_PIPELINES = 512;
constexpr uint32_t MAX_SHADERS = 512;

namespace ogfx {
  struct InitInfo;
  struct RenderPipelineDesc;

  //struct RenderPass {
  //  bool begin(WGPUDevice device, const RenderPipelineDesc& desc);
  //  void end();

  //  WGPURenderPassEncoder m_renderPass;
  //};

  struct RenderPipeline {
    bool create(WGPUDevice device, WGPUShaderModule shaderModule);
    void destroy();

    WGPURenderPipeline m_renderPipeline;
  };

  struct Shader {
    bool create(WGPUDevice device, Memory mem);
    void destroy();

    WGPUShaderModule m_shaderModule;
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
    ShaderHandle newShader(Memory mem);

    void beginDefaultPass();
    void endPass();
    void applyPipeline(RenderPipelineHandle handle);
    void draw();
    void commitFrame();

  private:
    WGPUInstance m_instance;
    WGPUSurface m_surface;
    WGPUAdapter m_adapter;
    std::vector<WGPUFeatureName> m_features;
    WGPUDevice m_device;
    WGPUQueue m_queue;
    WGPUSwapChain m_swapChain;
    WGPUCommandEncoder m_cmdEncoder;
    WGPUTextureView m_nextTexture;

    WGPURenderPassEncoder m_currentRenderPass;

    RenderPipeline m_renderPipelines[MAX_PIPELINES];
    HandleAllocator<RenderPipelineHandle> m_renderPipelineAlloc;
    Shader m_shaders[MAX_SHADERS];
    HandleAllocator<ShaderHandle> m_shaderAlloc;
    //RenderPass m_renderPasses[MAX_PASSES];
    //HandleAllocator<RenderPassHandle> m_renderPassAlloc;
  };
}