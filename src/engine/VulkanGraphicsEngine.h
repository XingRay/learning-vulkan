//
// Created by leixing on 2024/12/16.
//

#pragma once

#include <vector>

#include "engine/vulkan_wrapper/VulkanInstance.h"
#include "engine/vulkan_wrapper/VulkanSurface.h"
#include "engine/vulkan_wrapper/VulkanPhysicalDevice.h"
#include "engine/vulkan_wrapper/VulkanDevice.h"
#include "engine/vulkan_wrapper/VulkanShader.h"
#include "engine/vulkan_wrapper/VulkanSwapchain.h"
#include "engine/vulkan_wrapper/VulkanRenderPass.h"
#include "engine/vulkan_wrapper/VulkanPipeline.h"
#include "engine/vulkan_wrapper/VulkanCommandPool.h"
#include "engine/vulkan_wrapper/VulkanVertexBuffer.h"
#include "engine/vulkan_wrapper/VulkanIndexBuffer.h"
#include "engine/vulkan_wrapper/VulkanUniformBuffer.h"
#include "engine/vulkan_wrapper/VulkanSyncObject.h"
#include "engine/vulkan_wrapper/VulkanFrameBuffer.h"
#include "engine/vulkan_wrapper/VulkanSamplerBuffer.h"

#include "engine/common/StringListSelector.h"
#include "engine/VulkanPhysicalDeviceProvider.h"

#include <functional>
#include <memory>
#include <vector>

namespace engine {

    class VulkanGraphicsEngine {
    public:
        bool mFrameBufferResized = false;
        uint32_t mFrameCount = 2;
        uint32_t mCurrentFrame = 0;
        const std::array<float, 4> mClearColor = {0.05f, 0.05f, 0.05f, 1.0f};//{0.2f, 0.4f, 0.6f, 1.0f};
        const std::array<float, 4> mDepthStencil = {1.0f, 0, 0, 0};


        std::unique_ptr<VulkanInstance> mInstance;
        std::unique_ptr<VulkanSurface> mSurface;
        std::unique_ptr<VulkanPhysicalDevice> mPhysicalDevice;
        std::unique_ptr<VulkanDevice> mDevice;
        std::unique_ptr<VulkanShader> mShader;

        std::unique_ptr<VulkanSwapchain> mSwapchain;
        std::unique_ptr<VulkanRenderPass> mRenderPass;

        std::unique_ptr<VulkanPipeline> mPipeline;
        std::unique_ptr<VulkanCommandPool> mCommandPool;
        std::unique_ptr<VulkanFrameBuffer> mFrameBuffer;

        std::vector<std::unique_ptr<VulkanVertexBuffer>> mVulkanVertexBuffers;
        std::vector<vk::Buffer> mVertexBuffers;
        std::vector<vk::DeviceSize> mVertexBufferOffsets;

        std::unique_ptr<VulkanIndexBuffer> mIndexBuffer;

        std::unique_ptr<VulkanSyncObject> mSyncObject;

    public:
        VulkanGraphicsEngine(std::unique_ptr<VulkanInstance> vulkanInstance,
                             std::unique_ptr<VulkanSurface> vulkanSurface,
                             std::unique_ptr<VulkanPhysicalDevice> vulkanPhysicalDevice,
                             std::unique_ptr<VulkanDevice> vulkanDevice,
                             std::unique_ptr<VulkanCommandPool> commandPool,
                             std::unique_ptr<VulkanShader> vulkanShader,
                             uint32_t frameCount);

        ~VulkanGraphicsEngine();

        [[nodiscard]]
        vk::Instance getVKInstance() const;

        [[nodiscard]]
        vk::Device getVKDevice() const;

        void drawFrame();

        void createDirectlyTransferVertexBuffer(size_t size);

        void createStagingTransferVertexBuffer(size_t size);

        void updateVertexBuffer(const void *data, size_t size);

        void updateVertexBuffer(uint32_t index, const void *data, size_t size);

        template<typename T>
        void updateVertexBuffer(const std::vector<T> &data) {
            updateVertexBuffer(0, data);
        }

        template<typename T>
        void updateVertexBuffer(uint32_t index, const std::vector<T> &data) {
            if (index >= mVulkanVertexBuffers.size()) {
                LOG_E("index out of range, index:%d, size:%zu", index, mVulkanVertexBuffers.size());

                // Format the error message using std::to_string
                std::string errorMessage = "updateVertexBuffer: index out of range, index:" +
                                           std::to_string(index) +
                                           ", size:" +
                                           std::to_string(mVulkanVertexBuffers.size());
                throw std::runtime_error(errorMessage);
            }
            mVulkanVertexBuffers[index]->update(data);
        }

        void createDirectlyTransferIndexBuffer(size_t size);

        void createStagingTransferIndexBuffer(size_t size);

        void updateIndexBuffer(std::vector<uint32_t> indices) const;

        void updateUniformBuffer(uint32_t frameIndex, uint32_t set, uint32_t binding, void *data, uint32_t size);

        void updatePushConstant(uint32_t index, const void *data);

    private:
        void recreateSwapChain();
    };


} // engine
