#include "octogfx/octogfx.h"
#include "renderer_context.h"

namespace ogfx {
  static RendererContext m_ctx;

  bool Context::init(const InitInfo& info) {
    return m_ctx.init(info);
  }

  void Context::shutdown() {
    m_ctx.shutdown();
  }

  void Context::commitFrame() {
    m_ctx.commitFrame();
  }
}
