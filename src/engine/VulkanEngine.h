//
// Created by leixing on 2024/12/16.
//

#ifndef VULKANDEMO_VULKANENGINE_H
#define VULKANDEMO_VULKANENGINE_H

#include <vector>

#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanDescriptorSet.h"
#include "VulkanPipeline.h"
#include "VulkanCommandPool.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanSyncObject.h"
#include "VulkanFrameBuffer.h"

#include "ShaderData.h"

namespace engine {

    class VulkanEngine {
    public:
        bool mInitialized = false;
        bool mFrameBufferResized = false;
        const int MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t mCurrentFrame = 0;
        std::vector<app::Vertex> mVertices;
        std::vector<uint32_t> mIndices;

        const std::array<float, 4> mClearColor = {0.2f, 0.4f, 0.6f, 1.0f};
        const std::array<float, 4> mDepthStencil = {1.0f, 0, 0, 0};

        std::unique_ptr<VulkanInstance> mInstance;
        std::unique_ptr<VulkanSurface> mSurface;
        std::unique_ptr<VulkanDevice> mDevice;
        std::unique_ptr<VulkanSwapchain> mSwapchain;
        std::unique_ptr<VulkanRenderPass> mRenderPass;

        std::unique_ptr<VulkanDescriptorSet> mDescriptorSet;
        std::unique_ptr<VulkanPipeline> mPipeline;
        std::unique_ptr<VulkanCommandPool> mCommandPool;
        std::unique_ptr<VulkanFrameBuffer> mFrameBuffer;

        std::unique_ptr<VulkanVertexBuffer> mVertexBuffer;
        std::unique_ptr<VulkanIndexBuffer> mIndexBuffer;

        std::vector<VulkanUniformBuffer> mTransformUniformBuffers;
        std::vector<VulkanUniformBuffer> mColorUniformBuffers;

        std::unique_ptr<VulkanSyncObject> mSyncObject;

    public:
        explicit VulkanEngine(const std::vector<const char *> &instanceExtensions,
                              const std::vector<const char *> &layers,
                              int framesInFlight = 2);

        ~VulkanEngine();

        [[nodiscard]]
        vk::Instance getVKInstance() const;

        [[nodiscard]]
        vk::Device getVKDevice() const;

        // Initialize vulkan device context
        // after return, vulkan is ready to draw
        bool initVulkan(std::unique_ptr<VulkanSurface> &vulkanSurface,
                        const std::vector<const char *> &deviceExtensions,
                        const std::vector<char> &vertexShaderCode,
                        const std::vector<char> &fragmentShaderCode);

        // delete vulkan device context when application goes away
        void deleteVulkan();

        // Check if vulkan is ready to draw
        [[nodiscard]]
        bool isVulkanReady() const;

        // Ask Vulkan to Render a frame
        void drawFrame();

        void UpdateColor(float r, float g, float b);

        void recreateSwapChain();
    };

} // engine

#endif //VULKANDEMO_VULKANENGINE_H
