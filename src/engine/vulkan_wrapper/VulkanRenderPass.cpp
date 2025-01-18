//
// Created by leixing on 2024/12/26.
//

#include "VulkanRenderPass.h"

namespace engine {

    VulkanRenderPass::VulkanRenderPass(const VulkanDevice &vulkanDevice, const VulkanSwapchain &vulkanSwapchain) : mDevice(vulkanDevice) {
        vk::AttachmentDescription colorAttachmentDescription{};
        colorAttachmentDescription
                .setFormat(vulkanSwapchain.getDisplayFormat())
                .setSamples(vulkanDevice.getMsaaSamples())
                        //载入图像前将帧缓冲清0
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                        // 渲染图像之后将图像数据保存
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                        // 模版缓冲, 这里不关注
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                        // 常见的布局
                        //
                        //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL：用作彩色附件的图像
                        //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR：要在交换链中呈现的图像
                        //VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL：用作内存复制操作目标的图像
                        //
                        // initialLayout 渲染通道开始之前图像将具有的布局
                        // finalLayout 渲染通道完成时自动转换到的布局
                        //
                        // 使用 VK_IMAGE_LAYOUT_UNDEFINED 意味着我们不关心图像以前的布局
                        // 这个特殊值的警告是图像的内容不能保证被保留，但这并不重要，因为我们无论如何要清除
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentDescription depthAttachmentDescription{};
        depthAttachmentDescription
                .setFormat(findDepthFormat(vulkanDevice.getPhysicalDevice()))
                .setSamples(vulkanDevice.getMsaaSamples())
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);


        vk::AttachmentDescription colorAttachmentResolveDescription{};
        colorAttachmentResolveDescription
                .setFormat(vulkanSwapchain.getDisplayFormat())
                .setSamples(vk::SampleCountFlagBits::e1)
                .setLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        std::array<vk::AttachmentDescription, 3> attachments = {colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolveDescription};

        // 多个 colorAttachmentDescription 组成数组, 上面只有一个 colorAttachmentDescription, 那么下标为 0
        vk::AttachmentReference colorAttachmentReference{};
        colorAttachmentReference.setAttachment(0)
                .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentReference depthAttachmentReference{};
        depthAttachmentReference.setAttachment(1)
                .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::AttachmentReference colorAttachmentResolveReference{};
        colorAttachmentResolveReference.setAttachment(2)
                .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::SubpassDescription subpassDescription{};
        subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                        // 渲染通道可能需要绘制到多个颜色附件（例如，具有多重渲染目标（MRT）功能的场景）。
                .setColorAttachmentCount(1)
                .setPColorAttachments(&colorAttachmentReference)

                        // 一个子通道最多只能绑定一个深度模板附件。
                .setPDepthStencilAttachment(&depthAttachmentReference)

                        // pResolveAttachments 是用于存储多重采样 (MSAA) 图像解析结果的附件。它的存在是为了将一个多重采样的颜色附件解析为普通（单一采样）附件。
                        // pResolveAttachments 的数量 始终与 pColorAttachments 的数量一致，即每个颜色附件都可以有一个对应的解析附件。
                .setPResolveAttachments(&colorAttachmentResolveReference);


        vk::SubpassDependency subpassDependency = vk::SubpassDependency{}
                .setSrcSubpass(vk::SubpassExternal)
                .setDstSubpass(0)
                .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                .setSrcAccessMask(vk::AccessFlagBits::eNone)
                .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

        auto subpassDescriptions = std::array<vk::SubpassDescription, 1>{subpassDescription};
        vk::RenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo
                .setAttachments(attachments)
                .setSubpasses(subpassDescriptions)
//            .setDependencyCount(1)
//            .setPDependencies(&subpassDependency);
                .setDependencyCount(0)
                .setPDependencies(nullptr);

        mRenderPass = vulkanDevice.getDevice().createRenderPass(renderPassCreateInfo);
    }

    VulkanRenderPass::~VulkanRenderPass() {
        LOG_D("VulkanRenderPass::~VulkanRenderPass");
        mDevice.getDevice().destroy(mRenderPass);
    }

    const vk::RenderPass &VulkanRenderPass::getRenderPass() const {
        return mRenderPass;
    }

    vk::Format VulkanRenderPass::findDepthFormat(const vk::PhysicalDevice &physicalDevice) {
        return findSupportedFormat(
                physicalDevice,
                {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }

    vk::Format VulkanRenderPass::findSupportedFormat(const vk::PhysicalDevice &physicalDevice, const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
        for (const auto &format: candidates) {
            vk::FormatProperties properties = physicalDevice.getFormatProperties(format);
            if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format !");
    }
}