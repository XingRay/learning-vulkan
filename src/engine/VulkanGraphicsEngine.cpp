//
// Created by leixing on 2024/12/16.
//

#include <cassert>
#include <utility>
#include <string>

#include "engine/VulkanGraphicsEngine.h"
#include "engine/common/StringListSelector.h"
#include "engine/VkCheckCpp.h"
#include "engine/VkCheck.h"
#include "engine/Log.h"

namespace engine {

    VulkanGraphicsEngine::VulkanGraphicsEngine(std::unique_ptr<VulkanInstance> vulkanInstance,
                                               std::unique_ptr<VulkanSurface> vulkanSurface,
                                               std::unique_ptr<VulkanPhysicalDevice> vulkanPhysicalDevice,
                                               std::unique_ptr<VulkanDevice> vulkanDevice,
                                               std::unique_ptr<VulkanCommandPool> commandPool,
                                               std::unique_ptr<VulkanShader> vulkanShader,
                                               uint32_t frameCount) {
        mInstance = std::move(vulkanInstance);
        mSurface = std::move(vulkanSurface);
        mPhysicalDevice = std::move(vulkanPhysicalDevice);
        mDevice = std::move(vulkanDevice);
        mCommandPool = std::move(commandPool);
        mShader = std::move(vulkanShader);
        mFrameCount = frameCount;

        LOG_D("VulkanGraphicsEngine::init");
        if (mFrameCount <= 0) {
            throw std::runtime_error("mFrameCount<=0");
        }
        if (mInstance == nullptr) {
            throw std::runtime_error("mInstance== nullptr");
        }
        if (mSurface == nullptr) {
            throw std::runtime_error("mSurface== nullptr");
        }

        vk::Device device = mDevice->getDevice();

        vk::Extent2D currentExtent = mDevice->getCapabilities().currentExtent;
        LOG_D("currentExtent:[%d x %d]", currentExtent.width, currentExtent.height);
        mSwapchain = std::make_unique<VulkanSwapchain>(*mDevice, *mSurface, currentExtent.width, currentExtent.height);
        mRenderPass = std::make_unique<VulkanRenderPass>(*mDevice, *mSwapchain);

//        const std::vector<uint32_t> &uniformSizes = mShader->getUniforms()
//        LOG_D("create VulkanUniformBuffer");
//        for (int frameIndex = 0; frameIndex < mFrameCount; frameIndex++) {
//            std::vector<std::unique_ptr<VulkanUniformBuffer>> uniformBuffers;
//            uniformBuffers.resize(uniformSizes.size());
//            for (unsigned int uniformSize: uniformSizes) {
//                uniformBuffers.push_back(std::make_unique<VulkanUniformBuffer>(*mDevice, uniformSize));
//            }
//            mUniformBuffers.push_back(std::move(uniformBuffers));
//        }

//        const std::vector<ImageSize> &imageSizes = mShader->getSamplerImageSizes();
//        LOG_D("create VulkanTextureSampler");
//        for (int frameIndex = 0; frameIndex < mFrameCount; frameIndex++) {
//            std::vector<std::unique_ptr<VulkanTextureSampler>> samplers;
//            samplers.resize(imageSizes.size());
//            for (int j = 0; j < imageSizes.size(); j++) {
//                const ImageSize &imageSize = imageSizes[j];
//                samplers[j] = std::make_unique<VulkanTextureSampler>(*mDevice, *mCommandPool, imageSize.width, imageSize.height, imageSize.channels);
//            }
//            mTextureSamplers.push_back(std::move(samplers));
//        }

        LOG_D("create VulkanPipeline");
        mPipeline = std::make_unique<VulkanPipeline>(*mDevice, *mSwapchain, *mRenderPass, *mShader);
        LOG_D("create VulkanFrameBuffer");
        mFrameBuffer = std::make_unique<VulkanFrameBuffer>(*mDevice, *mSwapchain, *mRenderPass, *mCommandPool);
        LOG_D("create VulkanSyncObject");
        mSyncObject = std::make_unique<VulkanSyncObject>(*mDevice, mFrameCount);

//        if (mShader->getVertexPushConstantRange().size > 0) {
//            mVertexPushConstantData.resize(mShader->getVertexPushConstantRange().size);
//        }
//        if (mShader->getFragmentPushConstantRange().size > 0) {
//            mFragmentPushConstantData.resize(mShader->getFragmentPushConstantRange().size);
//        }
//        uint32_t totalPushConstantsSize = mShader->getVertexPushConstantRange().size + mShader->getFragmentPushConstantRange().size;
//        uint32_t pushConstantsSizeLimit = mDevice->getPhysicalDevice().getProperties().limits.maxPushConstantsSize;
//        if (totalPushConstantsSize > pushConstantsSizeLimit) {
//            LOG_E("totalPushConstantsSize > pushConstantsSizeLimit, totalPushConstantsSize:%d, pushConstantsSizeLimit:%d", totalPushConstantsSize, pushConstantsSizeLimit);
//            throw std::runtime_error("totalPushConstantsSize > pushConstantsSizeLimit");
//        }
    }

    VulkanGraphicsEngine::~VulkanGraphicsEngine() {
        LOG_D("~VulkanEngine()");

        mCommandPool.reset();

        mSyncObject.reset();

        mIndexBuffer.reset();
        mVertexBuffers.clear();
        mFrameBuffer.reset();

        mPipeline.reset();

        mRenderPass.reset();
        mSwapchain.reset();
        mDevice.reset();
        mSurface.reset();
        mInstance.reset();
    }

    vk::Instance VulkanGraphicsEngine::getVKInstance() const {
        return mInstance->getInstance();
    }

    vk::Device VulkanGraphicsEngine::getVKDevice() const {
        return mDevice->getDevice();
    }


    void VulkanGraphicsEngine::drawFrame() {
        const vk::Device device = mDevice->getDevice();

        vk::Result result = mSyncObject->waitFence(mCurrentFrame);
        if (result != vk::Result::eSuccess) {
            LOG_E("waitForFences failed");
            throw std::runtime_error("waitForFences failed");
        }

        auto [acquireResult, imageIndex] = device.acquireNextImageKHR(mSwapchain->getSwapChain(), std::numeric_limits<uint64_t>::max(), mSyncObject->getImageAvailableSemaphore(mCurrentFrame));
        if (acquireResult != vk::Result::eSuccess) {
            if (acquireResult == vk::Result::eErrorOutOfDateKHR) {
                // 交换链已与表面不兼容，不能再用于渲染。通常在窗口大小调整后发生。
                LOG_E("acquireNextImageKHR: eErrorOutOfDateKHR, recreateSwapChain");
                recreateSwapChain();
                return;
            } else if (acquireResult == vk::Result::eSuboptimalKHR) {
                //vk::Result::eSuboptimalKHR 交换链仍然可以成功显示到表面，但表面属性不再完全匹配。
                LOG_D("acquireNextImageKHR: eSuboptimalKHR");
            } else {
                LOG_E("acquireNextImageKHR: failed: %d", acquireResult);
                throw std::runtime_error("acquireNextImageKHR failed");
            }
        }

        // 检查当前帧是否已经在使用
        vk::CommandBuffer currentFrameCommandBuffer = mCommandPool->getCommandBuffers()[mCurrentFrame];
        currentFrameCommandBuffer.reset();
        mCommandPool->recordCommandInRenderPass(currentFrameCommandBuffer,
                                                mFrameBuffer->getFrameBuffers()[imageIndex],
                                                mRenderPass->getRenderPass(),
                                                mPipeline->getPipeline(),
                                                mSwapchain->getDisplaySize(),
                                                mClearColor,
                                                mDepthStencil,
                                                [&](vk::CommandBuffer commandBuffer) -> void {
                                                    commandBuffer.bindVertexBuffers(0, mVertexBuffers, mVertexBufferOffsets);
                                                    commandBuffer.bindIndexBuffer(mIndexBuffer->getIndexBuffer(), 0, vk::IndexType::eUint32);
                                                    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipeline->getPipelineLayout(), 0,
                                                                                     mShader->getDescriptorSets(mCurrentFrame), nullptr);

                                                    // push constants
                                                    const std::vector<vk::PushConstantRange> &pushConstantRanges = mShader->getPushConstantRanges();
                                                    const std::vector<std::vector<uint8_t>> &pushConstantDataList = mShader->getPushConstantDataList();
                                                    if (!pushConstantRanges.empty()) {
                                                        for (uint32_t pushConstantIndex = 0; pushConstantIndex < pushConstantRanges.size(); pushConstantIndex++) {
                                                            const vk::PushConstantRange &pushConstantRange = pushConstantRanges[pushConstantIndex];
                                                            const std::vector<uint8_t> pushConstantData = pushConstantDataList[pushConstantIndex];

                                                            commandBuffer.pushConstants(mPipeline->getPipelineLayout(), pushConstantRange.stageFlags,
                                                                                        pushConstantRange.offset,
                                                                                        pushConstantRange.size,
                                                                                        pushConstantData.data());
                                                        }
                                                    }

                                                    // draw call
                                                    commandBuffer.drawIndexed(mIndexBuffer->getIndicesCount(), 1, 0, 0, 0);
                                                });

        result = mSyncObject->resetFence(mCurrentFrame);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("resetFences failed");
        }

        std::array<vk::Semaphore, 1> waitSemaphores = {mSyncObject->getImageAvailableSemaphore(mCurrentFrame)};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        std::array<vk::Semaphore, 1> signalSemaphores = {mSyncObject->getRenderFinishedSemaphore(mCurrentFrame)};
        std::array<vk::CommandBuffer, 1> commandBuffers = {currentFrameCommandBuffer};

        vk::SubmitInfo submitInfo{};
        submitInfo
                .setWaitSemaphores(waitSemaphores)
                .setPWaitDstStageMask(waitStages)
                .setCommandBuffers(commandBuffers)
                .setSignalSemaphores(signalSemaphores);

        std::array<vk::SubmitInfo, 1> submitInfos = {submitInfo};
        mDevice->getGraphicsQueue().submit(submitInfos, mSyncObject->getFence(mCurrentFrame));

        std::array<vk::SwapchainKHR, 1> swapchains = {mSwapchain->getSwapChain()};
        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(signalSemaphores)
                .setSwapchains(swapchains)
                .setImageIndices(imageIndex);

//    std::cout << "presentKHR, mFrameBufferResized:" << mFrameBufferResized << std::endl;

        // https://github.com/KhronosGroup/Vulkan-Hpp/issues/599
        // 当出现图片不匹配时， cpp风格的 presentKHR 会抛出异常， 而不是返回 result， 而C风格的 presentKHR 接口会返回 result
        try {
            result = mDevice->getPresentQueue().presentKHR(presentInfo);
        } catch (const vk::OutOfDateKHRError &e) {
            LOG_E("mPresentQueue.presentKHR => OutOfDateKHRError");
            result = vk::Result::eErrorOutOfDateKHR;
        }

        if (result != vk::Result::eSuccess) {
            if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFrameBufferResized) {
                mFrameBufferResized = false;
                LOG_E("presentKHR: eErrorOutOfDateKHR or eSuboptimalKHR or mFrameBufferResized, recreateSwapChain");
                recreateSwapChain();
                return;
            } else {
                throw std::runtime_error("presentKHR failed");
            }
        }

        mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
    }

    void VulkanGraphicsEngine::createDirectlyTransferVertexBuffer(size_t size) {
        std::unique_ptr<DirectlyTransferVertexBuffer> vertexBuffer = std::make_unique<DirectlyTransferVertexBuffer>(*mDevice, size);
        mVulkanVertexBuffers.push_back(std::move(vertexBuffer));
        mVertexBuffers.push_back(mVulkanVertexBuffers.back()->getVertexBuffer());
        mVertexBufferOffsets.push_back(0);
    }

    void VulkanGraphicsEngine::createStagingTransferVertexBuffer(size_t size) {
        std::unique_ptr<StagingTransferVertexBuffer> vertexBuffer = std::make_unique<StagingTransferVertexBuffer>(*mDevice, *mCommandPool, size);
        mVulkanVertexBuffers.push_back(std::move(vertexBuffer));
        mVertexBuffers.push_back(mVulkanVertexBuffers.back()->getVertexBuffer());
        mVertexBufferOffsets.push_back(0);
    }

    void VulkanGraphicsEngine::updateVertexBuffer(const void *data, size_t size) {
        updateVertexBuffer(0, data, size);
    }

    void VulkanGraphicsEngine::updateVertexBuffer(uint32_t index, const void *data, size_t size) {
        if (index >= mVulkanVertexBuffers.size()) {
            LOG_E("index out of range, index:%d, size:%zu", index, mVulkanVertexBuffers.size());

            // Format the error message using std::to_string
            std::string errorMessage = "updateVertexBuffer: index out of range, index:" +
                                       std::to_string(index) +
                                       ", size:" +
                                       std::to_string(mVulkanVertexBuffers.size());
            throw std::runtime_error(errorMessage);
        }
        mVulkanVertexBuffers[index]->update(data, size);
    }

    void VulkanGraphicsEngine::createDirectlyTransferIndexBuffer(size_t size) {
        mIndexBuffer.reset();
        mIndexBuffer = std::make_unique<DirectlyTransferIndexBuffer>(*mDevice, size);
    }

    void VulkanGraphicsEngine::createStagingTransferIndexBuffer(size_t size) {
        mIndexBuffer.reset();
        mIndexBuffer = std::make_unique<StagingTransferIndexBuffer>(*mDevice, *mCommandPool, size);
    }

    void VulkanGraphicsEngine::updateIndexBuffer(std::vector<uint32_t> indices) const {
        mIndexBuffer->update(std::move(indices));
    }

    void VulkanGraphicsEngine::updateUniformBuffer(uint32_t frameIndex, uint32_t set, uint32_t binding, void *data, uint32_t size) {
        mShader->updateBuffer(frameIndex, set, binding, data, size);
    }

    void VulkanGraphicsEngine::updatePushConstant(uint32_t index, const void *data) {
        mShader->updatePushConstant(index, data);
    }

    void VulkanGraphicsEngine::recreateSwapChain() {
        throw std::runtime_error("recreateSwapChain");
    }

} // engine
