//
// Created by leixing on 2024/12/30.
//

#include "VulkanCommandPool.h"
#include "engine/Log.h"

namespace engine {

    VulkanCommandPool::VulkanCommandPool(const VulkanDevice &device, uint32_t commandBufferCount)
            : mDevice(device) {

        vk::CommandPoolCreateInfo commandPoolCreateInfo{};
        uint32_t queueFamilyIndex = device.getGraphicQueueFamilyIndex();
        commandPoolCreateInfo
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                .setQueueFamilyIndex(queueFamilyIndex);

        mCommandPool = device.getDevice().createCommandPool(commandPoolCreateInfo);

        vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
        commandBufferAllocateInfo.setCommandPool(mCommandPool)
                .setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandBufferCount(commandBufferCount);

        mCommandBuffers = mDevice.getDevice().allocateCommandBuffers(commandBufferAllocateInfo);
    }

    VulkanCommandPool::~VulkanCommandPool() {
        LOG_D("VulkanCommandPool::~VulkanCommandPool()");
        mDevice.getDevice().freeCommandBuffers(mCommandPool, mCommandBuffers);
        mDevice.getDevice().destroy(mCommandPool);
    }

    const vk::CommandPool &VulkanCommandPool::getCommandPool() const {
        return mCommandPool;
    }

    const std::vector<vk::CommandBuffer> &VulkanCommandPool::getCommandBuffers() const {
        return mCommandBuffers;
    }

    void VulkanCommandPool::recordCommandInRenderPass(const vk::CommandBuffer &commandBuffer,
                                                      const vk::Framebuffer &frameBuffer,
                                                      const vk::RenderPass &renderPass,
                                                      const vk::Pipeline &pipeline,
                                                      const vk::Extent2D &displaySize,
                                                      const std::array<float, 4> &clearColor,
                                                      const std::array<float, 4> &depthStencil,
                                                      const std::function<void(const vk::CommandBuffer &)> &command) {

//        LOG_D("VulkanCommandPool::recordCommandInRenderPass");
//        LOG_D("displaySize: [ %d x %d ]", displaySize.width, displaySize.height);
        vk::Rect2D renderArea{};
        renderArea
                .setOffset(vk::Offset2D{0, 0})
                .setExtent(displaySize);

        vk::ClearValue colorClearValue = vk::ClearValue{clearColor};
        vk::ClearValue depthStencilClearValue = vk::ClearValue{vk::ClearColorValue(depthStencil)};
        std::array<vk::ClearValue, 2> clearValues = {colorClearValue, depthStencilClearValue};

        vk::Viewport viewport{};
        viewport.setX(0.0f)
                .setY(0.0f)
                .setWidth((float) displaySize.width)
                .setHeight((float) displaySize.height)
                .setMinDepth(0.0f)
                .setMaxDepth(1.0f);

        vk::Rect2D scissor{};
        scissor.setOffset(vk::Offset2D{0, 0})
                .setExtent(displaySize);

        vk::CommandBufferBeginInfo commandBufferBeginInfo;
        commandBufferBeginInfo
                .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
                .setPInheritanceInfo(nullptr);

        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo
                .setRenderPass(renderPass)
                .setFramebuffer(frameBuffer)
                .setRenderArea(renderArea)
                .setClearValues(clearValues);

        commandBuffer.begin(commandBufferBeginInfo);
        commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

        commandBuffer.setViewport(0, 1, &viewport);
        commandBuffer.setScissor(0, 1, &scissor);

        command(commandBuffer);

        commandBuffer.endRenderPass();
        commandBuffer.end();
    }

    vk::CommandBuffer VulkanCommandPool::allocateCommand() const {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo
                .setLevel(vk::CommandBufferLevel::ePrimary)
                .setCommandPool(mCommandPool)
                .setCommandBufferCount(1);

        return mDevice.getDevice().allocateCommandBuffers(commandBufferAllocateInfo)[0];
    }

    void VulkanCommandPool::submitCommand(const vk::CommandBuffer &commandBuffer) const {
        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBufferCount(1)
                .setPCommandBuffers(&commandBuffer);

        mDevice.getGraphicsQueue().submit(submitInfo);
        mDevice.getGraphicsQueue().waitIdle();

        mDevice.getDevice().freeCommandBuffers(mCommandPool, commandBuffer);
    }

    void VulkanCommandPool::submitOneTimeCommand(const std::function<void(const vk::CommandBuffer &)> &command) const {
        vk::CommandBuffer commandBuffer = allocateCommand();
        recordCommand(commandBuffer, vk::CommandBufferUsageFlagBits::eOneTimeSubmit, command);
        submitCommand(commandBuffer);
    }

    void VulkanCommandPool::recordCommand(const vk::CommandBuffer &commandBuffer, vk::CommandBufferUsageFlagBits usage, const std::function<void(const vk::CommandBuffer &)> &command) {
        vk::CommandBufferBeginInfo commandBufferBeginInfo;
        commandBufferBeginInfo
                .setFlags(usage)
                .setPInheritanceInfo(nullptr);

        commandBuffer.begin(commandBufferBeginInfo);


        command(commandBuffer);


        commandBuffer.end();
    }
} // engine