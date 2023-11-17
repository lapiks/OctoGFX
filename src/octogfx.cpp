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

  RenderPipelineHandle Context::newRenderPipeline(const RenderPipelineDesc& desc) {
    return m_ctx.newRenderPipeline(desc);
  }

  void Context::beginDefaultPass() {
    m_ctx.beginDefaultPass();
  }

  void Context::endPass() {
    m_ctx.endPass();
  }

  void Context::applyPipeline(RenderPipelineHandle handle) {
    m_ctx.applyPipeline(handle);
  }

  void Context::draw() {
    m_ctx.draw();
  }

  void Context::commitFrame() {
    m_ctx.commitFrame();
  }
}
