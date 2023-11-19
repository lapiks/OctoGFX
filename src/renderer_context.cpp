#include "renderer_context.h"
#include <iostream>
#include <cassert>
#include <Windows.h>

namespace ogfx {
  WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const* options) {
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData {
      WGPUAdapter adapter = nullptr;
      bool requestEnded = false;
    };
    UserData userData;

    // Callback called by wgpuInstanceRequestAdapter when the request returns
    // This is a C++ lambda function, but could be any function defined in the
    // global scope. It must be non-capturing (the brackets [] are empty) so
    // that it behaves like a regular C function pointer, which is what
    // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
    // is to convey what we want to capture through the pUserData pointer,
    // provided as the last argument of wgpuInstanceRequestAdapter and received
    // by the callback as its last argument.
    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* pUserData) {
      UserData& userData = *reinterpret_cast<UserData*>(pUserData);
      if (status == WGPURequestAdapterStatus_Success) {
        userData.adapter = adapter;
      }
      else {
        std::cout << "Could not get WebGPU adapter: " << message << std::endl;
      }
      userData.requestEnded = true;
      };

    // Call to the WebGPU request adapter procedure
    wgpuInstanceRequestAdapter(
      instance /* equivalent of navigator.gpu */,
      options,
      onAdapterRequestEnded,
      (void*)&userData
    );

    // In theory we should wait until onAdapterReady has been called, which
    // could take some time (what the 'await' keyword does in the JavaScript
    // code). In practice, we know that when the wgpuInstanceRequestAdapter()
    // function returns its callback has been called.
    assert(userData.requestEnded);

    return userData.adapter;
  }

  WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor) {
    struct UserData {
      WGPUDevice device = nullptr;
      bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* pUserData) {
      UserData& userData = *reinterpret_cast<UserData*>(pUserData);
      if (status == WGPURequestDeviceStatus_Success) {
        userData.device = device;
      }
      else {
        std::cout << "Could not get WebGPU device: " << message << std::endl;
      }
      userData.requestEnded = true;
      };

    wgpuAdapterRequestDevice(
      adapter,
      descriptor,
      onDeviceRequestEnded,
      (void*)&userData
    );

    assert(userData.requestEnded);

    return userData.device;
  }

  WGPUInstance createInstance() {
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    return wgpuCreateInstance(&desc);
  }

  WGPUSurface createSurface(WGPUInstance instance, const PlatformData& platformData) {
    // for windows
    HINSTANCE hinstance = GetModuleHandle(NULL);

    WGPUChainedStruct chainedStruct;
    chainedStruct.next = NULL;
    chainedStruct.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND;

    WGPUSurfaceDescriptorFromWindowsHWND surfaceDescriptorFromWindowsHWND;
    surfaceDescriptorFromWindowsHWND.chain = chainedStruct;
    surfaceDescriptorFromWindowsHWND.hinstance = hinstance;
    surfaceDescriptorFromWindowsHWND.hwnd = platformData.nativeWindowHandle;

    WGPUSurfaceDescriptor surfaceDescriptor;
    surfaceDescriptor.label = NULL;
    surfaceDescriptor.nextInChain = (const WGPUChainedStruct*)&(surfaceDescriptorFromWindowsHWND);

    return wgpuInstanceCreateSurface(
      instance,
      &surfaceDescriptor
    );

    // todo: other platforms
  }

  WGPUAdapter createAdapter(WGPUInstance instance, WGPUSurface surface) {
    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = surface;
    WGPUAdapter adapter = requestAdapter(instance, &adapterOpts);

    return adapter;
  }

  WGPUDevice createDevice(WGPUAdapter adapter) {
    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = "Device"; // anything works here, that's your call
    deviceDesc.requiredFeaturesCount = 0; // we do not require any specific feature
    deviceDesc.requiredLimits = nullptr; // we do not require any specific limit
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "Default queue";
    WGPUDevice device = requestDevice(adapter, &deviceDesc);

    return device;
  }

  WGPUSwapChain createSwapChain(WGPUDevice device, WGPUSurface surface) {
    WGPUSwapChainDescriptor swapChainDesc = {};
    swapChainDesc.nextInChain = nullptr;
    swapChainDesc.width = 640;
    swapChainDesc.height = 480;
    swapChainDesc.usage = WGPUTextureUsage_RenderAttachment;
    swapChainDesc.presentMode = WGPUPresentMode_Fifo;

#ifdef WEBGPU_BACKEND_DAWN
    swapChainDesc.format = WGPUTextureFormat_BGRA8Unorm;
#else 
    swapChainDesc.format = wgpuSurfaceGetPreferredFormat(surface, adapter);
#endif

    WGPUSwapChain swapChain = wgpuDeviceCreateSwapChain(device, surface, &swapChainDesc);


    return swapChain;
  }

  WGPUCommandEncoder createCmdEncoder(WGPUDevice device) {
    WGPUCommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    encoderDesc.label = "Command encoder";
    return wgpuDeviceCreateCommandEncoder(device, &encoderDesc);
  }

  std::vector<WGPUFeatureName> retrieveFeatures(WGPUAdapter adapter) {
    std::vector<WGPUFeatureName> features;

    // Call the function a first time with a null return address, just to get
    // the entry count.
    size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);
    features.resize(featureCount);

    // Call the function a second time, with a non-null return address
    wgpuAdapterEnumerateFeatures(adapter, features.data());

    return features;
  }

  bool RendererContext::init(const InitInfo& info) {
    m_instance = createInstance();
    if (!m_instance) {
      std::cerr << "Could not initialize WebGPU!" << std::endl;
      return false;
    }

    m_surface = createSurface(m_instance, info.platformData);

    std::cout << "Requesting adapter..." << std::endl;
    m_adapter = createAdapter(m_instance, m_surface);
    if (!m_adapter) {
      std::cerr << "Adapter request failed" << std::endl;
      return false;
    }
    std::cout << "Got adapter: " << m_adapter << std::endl;

    m_features = retrieveFeatures(m_adapter);

    std::cout << "Adapter features:" << std::endl;
    for (auto f : m_features) {
      std::cout << " - " << f << std::endl;

    }

    std::cout << "Requesting device..." << std::endl;
    m_device = createDevice(m_adapter);
    if (!m_device) {
      std::cerr << "Device request failed" << std::endl;
      return false;
    }
    std::cout << "Got device: " << m_device << std::endl;

    auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
      std::cout << "Uncaptured device error: type " << type;
      if (message) std::cout << " (" << message << ")";
      std::cout << std::endl;
      };
    wgpuDeviceSetUncapturedErrorCallback(m_device, onDeviceError, nullptr /* pUserData */);

    m_queue = wgpuDeviceGetQueue(m_device);

    m_swapChain = createSwapChain(m_device, m_surface);
    if (!m_swapChain) {
      std::cerr << "Swap chain creation failed" << std::endl;
      return false;
    }
    std::cout << "Swapchain: " << m_swapChain << std::endl;

    m_cmdEncoder = createCmdEncoder(m_device);

    return true;
  }

  void RendererContext::shutdown() {
    wgpuSwapChainRelease(m_swapChain);
    wgpuDeviceRelease(m_device);
    wgpuSurfaceRelease(m_surface);
    wgpuAdapterRelease(m_adapter);
    wgpuInstanceRelease(m_instance);
  }

  RenderPipelineHandle RendererContext::newRenderPipeline(const RenderPipelineDesc& desc) {
    RenderPipelineHandle handle;
    m_renderPipelineAlloc.allocate(handle);

    const Shader& shader = m_shaders[desc.shader.id];

    m_renderPipelines[handle.id].create(m_device, shader.m_shaderModule);

    return handle;
  }

  ShaderHandle RendererContext::newShader(Memory mem) {
    ShaderHandle handle;
    m_shaderAlloc.allocate(handle);

    m_shaders[handle.id].create(m_device, mem);

    return handle;
  }

  void RendererContext::beginDefaultPass() {
    // Get texture view from the swap chain
    m_nextTexture = wgpuSwapChainGetCurrentTextureView(m_swapChain);
    if (!m_nextTexture) {
      std::cerr << "Cannot acquire next swap chain texture" << std::endl;
      return;
    }

    // Define attachments
    WGPURenderPassColorAttachment renderPassColorAttachment = {};
    renderPassColorAttachment.view = m_nextTexture;
    renderPassColorAttachment.resolveTarget = nullptr;
    renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
    renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
    renderPassColorAttachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };

    // Define Render Pass
    WGPURenderPassDescriptor renderPassDesc = {};
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &renderPassColorAttachment;
    renderPassDesc.depthStencilAttachment = nullptr;
    renderPassDesc.timestampWriteCount = 0; // for measurements
    renderPassDesc.timestampWrites = nullptr; // for measurements
    renderPassDesc.nextInChain = nullptr;

    m_currentRenderPass = wgpuCommandEncoderBeginRenderPass(m_cmdEncoder, &renderPassDesc);
  }

  void RendererContext::endPass() {
    wgpuRenderPassEncoderEnd(m_currentRenderPass);
    wgpuRenderPassEncoderRelease(m_currentRenderPass); // useful? 
  }

  void RendererContext::applyPipeline(RenderPipelineHandle handle) {
    const RenderPipeline& pipe = m_renderPipelines[handle.id];

    wgpuRenderPassEncoderSetPipeline(m_currentRenderPass, pipe.m_renderPipeline);
  }

  void RendererContext::draw() {
    wgpuRenderPassEncoderDraw(m_currentRenderPass, 3, 1, 0, 0);
  }

  void RendererContext::commitFrame() {
    // Create command buffer from encoder
    WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = "Command buffer";
    WGPUCommandBuffer command = wgpuCommandEncoderFinish(m_cmdEncoder, &cmdBufferDescriptor);

    // Submit the command queue
    wgpuQueueSubmit(m_queue, 1, &command);

#ifdef WEBGPU_BACKEND_DAWN
    wgpuCommandEncoderRelease(m_cmdEncoder);
    wgpuCommandBufferRelease(command);
#endif

    wgpuTextureViewRelease(m_nextTexture);

    wgpuSwapChainPresent(m_swapChain);

    m_cmdEncoder = createCmdEncoder(m_device);
  }

  bool RenderPipeline::create(WGPUDevice device, WGPUShaderModule shaderModule) {
    WGPURenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.nextInChain = nullptr;
    pipelineDesc.vertex.bufferCount = 0;
    pipelineDesc.vertex.buffers = nullptr;
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;
    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;

    WGPUFragmentState fragmentState{};
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    pipelineDesc.fragment = &fragmentState;

    pipelineDesc.depthStencil = nullptr;

    WGPUBlendState blendState{};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;
    blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
    blendState.alpha.dstFactor = WGPUBlendFactor_One;
    blendState.alpha.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTarget{};
    colorTarget.format = WGPUTextureFormat_BGRA8Unorm; // todo: parametrize (for wgpu)
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    // Samples per pixel
    pipelineDesc.multisample.count = 1;
    // Default value for the mask, meaning "all bits on"
    pipelineDesc.multisample.mask = ~0u;
    // Default value as well (irrelevant for count = 1 anyways)
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    pipelineDesc.layout = nullptr;

    m_renderPipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

    return true;
  }

  void RenderPipeline::destroy() {

  }

  bool Shader::create(WGPUDevice device, Memory mem) {
    WGPUShaderModuleDescriptor shaderDesc{};
#ifdef WEBGPU_BACKEND_WGPU
    shaderDesc.hintCount = 0;
    shaderDesc.hints = nullptr;
#endif

    WGPUShaderModuleWGSLDescriptor shaderCodeDesc{};
    // Set the chained struct's header
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderCodeDesc.code = reinterpret_cast<const char*>(mem.data);

    // Connect the chain
    shaderDesc.nextInChain = &shaderCodeDesc.chain;

    m_shaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    return true;
  }

  void Shader::destroy() {
  
  }

}
