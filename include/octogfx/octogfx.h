#pragma once

#include <stdint.h>

namespace ogfx {
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

  struct Context {
    bool init(const InitInfo& info);
    void shutdown();

    void commitFrame();
  };
}