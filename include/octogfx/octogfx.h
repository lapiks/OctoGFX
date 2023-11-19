#pragma once

#include <stdint.h>

namespace ogfx {
  constexpr uint16_t nullHandle = UINT16_MAX;

  #define OGFX_HANDLE(name) \
	struct name { uint16_t id = nullHandle; };

  OGFX_HANDLE(RenderPassHandle)
  OGFX_HANDLE(RenderPipelineHandle)
  OGFX_HANDLE(ShaderHandle)
  OGFX_HANDLE(BufferHandle)

  struct PlatformData {
    void* nativeWindowHandle = nullptr;
  };

  struct Resolution {
    uint32_t width = 640;
    uint32_t height = 480;
  };

  struct InitInfo {
    PlatformData platformData;
    Resolution resolution;
  };

  struct RenderPipelineDesc {
    ShaderHandle shader;
  };

  struct Memory {
    const uint8_t* data = nullptr;
    uint64_t size = 0;
  };

  struct Context {
    bool init(const InitInfo& info);
    void shutdown();

    RenderPipelineHandle newRenderPipeline(const RenderPipelineDesc& desc);
    ShaderHandle newShader(Memory mem);
    BufferHandle newBuffer(Memory mem);

    void beginDefaultPass();
    void endPass();
    void applyPipeline(RenderPipelineHandle handle);
    void draw();
    void commitFrame();
  };
}