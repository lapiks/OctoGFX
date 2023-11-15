#pragma once

#include <stdint.h>

namespace ogfx {
  constexpr uint16_t nullHandle = UINT16_MAX;

  #define OGFX_HANDLE(name) \
	struct name { uint16_t id = nullHandle; };

  OGFX_HANDLE(RenderPassHandle)
  OGFX_HANDLE(RenderPipelineHandle)

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

  };

  struct Context {
    bool init(const InitInfo& info);
    void shutdown();

    RenderPipelineHandle newRenderPipeline(const RenderPipelineDesc& desc);

    void applyPipeline(RenderPipelineHandle handle);
    void commitFrame();
  };
}