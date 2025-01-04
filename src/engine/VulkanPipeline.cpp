//
// Created by leixing on 2024/12/28.
//

#include "VulkanPipeline.h"
#include "Log.h"

namespace engine {
    VulkanPipeline::VulkanPipeline(const VulkanDevice &vulkanDevice,
                                   const VulkanSwapchain &swapchain,
                                   const VulkanDescriptorSet &descriptorSet,
                                   const VulkanRenderPass &renderPass,
                                   const vk::ShaderModule &vertexModule,
                                   const vk::ShaderModule &fragmentModule,
                                   const std::vector<vk::VertexInputBindingDescription> &vertexInputBindingDescriptions,
                                   const std::vector<vk::VertexInputAttributeDescription> &vertexInputAttributeDescriptions)
            : mDevice(vulkanDevice) {
        vk::Device device = vulkanDevice.getDevice();

        // input assembler
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
        inputAssemblyStateCreateInfo
                .setTopology(vk::PrimitiveTopology::eTriangleList)
                .setPrimitiveRestartEnable(vk::False);

        std::vector<vk::DynamicState> dynamicStages = {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor
        };

        vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo
                .setDynamicStates(dynamicStages);

        vk::Extent2D displaySize = swapchain.getDisplaySize();
        vk::Viewport viewport;
        viewport.setX(0.0f)
                .setY(0.0f)
                .setWidth((float) displaySize.width)
                .setHeight((float) displaySize.height)
                .setMinDepth(0.0f)
                .setMaxDepth(1.0f);

        vk::Rect2D scissor{};
        scissor.setOffset(vk::Offset2D{0, 0})
                .setExtent(displaySize);

        vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
        viewportStateCreateInfo.setViewportCount(1)
                .setPViewports(&viewport)
                .setScissorCount(1)
                .setPScissors(&scissor);

        // vertex shader
        vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
        vertexInputStateCreateInfo
                .setVertexBindingDescriptions(vertexInputBindingDescriptions)
                .setVertexAttributeDescriptions(vertexInputAttributeDescriptions);

        vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo;
        vertexShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eVertex)
                .setModule(vertexModule)
                .setPName("main")
                .setPSpecializationInfo(nullptr);

        // tessellation

        // geometry shader

        // rasterization
        vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;

        // 如果depthClampEnable设置为VK_TRUE，则超出近平面和远平面的片段将被夹紧，而不是丢弃它们。这在某些特殊情况下很有用，例如阴影贴图。使用此功能需要启用 GPU 功能。
        rasterizationStateCreateInfo.setDepthClampEnable(vk::False)
                        // 如果rasterizerDiscardEnable设置为VK_TRUE，则几何图形永远不会通过光栅化阶段。这基本上禁用了帧缓冲区的任何输出。
                .setRasterizerDiscardEnable(vk::False)
                        //确定polygonMode如何为几何体生成片段。可以使用以下模式：
                        //VK_POLYGON_MODE_FILL：用碎片填充多边形区域
                        //VK_POLYGON_MODE_LINE：多边形边缘绘制为线
                        //VK_POLYGON_MODE_POINT：多边形顶点绘制为点
                .setPolygonMode(vk::PolygonMode::eFill)
                        // 使用填充以外的任何模式都需要设置 lineWidth :
                .setLineWidth(1.0f)
                        // 设置面剔除策略, 这里设置为反面被剔除
                .setCullMode(vk::CullModeFlagBits::eBack)
//                .setCullMode(vk::CullModeFlagBits::eNone)
                        // 设置正面的方向
                .setFrontFace(vk::FrontFace::eCounterClockwise)
                .setDepthBiasEnable(vk::False)
                .setDepthBiasConstantFactor(0.0f)
                .setDepthBiasClamp(0.0f)
                .setDepthBiasSlopeFactor(0.0f);

        // depth & stencil testing
        vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
        depthStencilStateCreateInfo
//                .setDepthTestEnable(vk::True)
                .setDepthTestEnable(vk::False)
//                .setDepthWriteEnable(vk::True)
                .setDepthWriteEnable(vk::False)
                .setDepthCompareOp(vk::CompareOp::eLess)
                .setDepthBoundsTestEnable(vk::False)
                .setMinDepthBounds(0.0f)
                .setMaxDepthBounds(1.0f)
                .setStencilTestEnable(vk::False)
                .setFront(vk::StencilOpState{})
                .setBack(vk::StencilOpState{});


        // Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
        multisampleStateCreateInfo.setSampleShadingEnable(vk::True)
                .setRasterizationSamples(vulkanDevice.getMsaaSamples())
                .setMinSampleShading(0.2f)
                .setPSampleMask(nullptr)
                .setAlphaToCoverageEnable(vk::False)
                .setAlphaToOneEnable(vk::False);

        // fragment shader
        vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo;
        fragmentShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eFragment)
                .setModule(fragmentModule)
                .setPName("main")
                .setPSpecializationInfo(nullptr);

        // color blending
        vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{};
        colorBlendAttachmentState
//                .setBlendEnable(vk::True)
                .setBlendEnable(vk::False)
                .setColorWriteMask(
                        vk::ColorComponentFlagBits::eR
                        | vk::ColorComponentFlagBits::eG
                        | vk::ColorComponentFlagBits::eB
                        | vk::ColorComponentFlagBits::eA)
                .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
                .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
                .setColorBlendOp(vk::BlendOp::eAdd)
                .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                .setAlphaBlendOp(vk::BlendOp::eAdd);

        std::array<vk::PipelineColorBlendAttachmentState, 1> colorBlendAttachmentStates = {colorBlendAttachmentState};
        vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
        colorBlendStateCreateInfo.setLogicOpEnable(vk::False)
                .setLogicOp(vk::LogicOp::eCopy)
                .setAttachments(colorBlendAttachmentStates)
                .setBlendConstants(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f});


        std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts = {descriptorSet.getDescriptorSetLayout()};
        vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo
                .setSetLayouts(descriptorSetLayouts)
                .setPushConstantRangeCount(0)
                .setPPushConstantRanges(nullptr);

        mPipelineLayout = vulkanDevice.getDevice().createPipelineLayout(pipelineLayoutCreateInfo);

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfos = {vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo};
        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
        graphicsPipelineCreateInfo
                .setStages(shaderStageCreateInfos)
                .setPVertexInputState(&vertexInputStateCreateInfo)
                .setPInputAssemblyState(&inputAssemblyStateCreateInfo)
                .setPViewportState(&viewportStateCreateInfo)
                .setPRasterizationState(&rasterizationStateCreateInfo)
                .setPMultisampleState(&multisampleStateCreateInfo)
                .setPDepthStencilState(&depthStencilStateCreateInfo)
                .setPColorBlendState(&colorBlendStateCreateInfo)
                .setPDynamicState(&dynamicStateCreateInfo)
                .setLayout(mPipelineLayout)
                .setRenderPass(renderPass.getRenderPass())
                .setSubpass(0)
                .setBasePipelineHandle(nullptr)
                .setBasePipelineIndex(-1);

        auto [result, pipeline] = device.createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("createGraphicsPipelines failed");
        }
        mPipeline = pipeline;
    }

    VulkanPipeline::~VulkanPipeline() {
        LOG_D("VulkanPipeline::~VulkanPipeline");
        vk::Device device = mDevice.getDevice();
        device.destroy(mPipeline);
        device.destroy(mPipelineLayout);
    }

    const vk::Pipeline &VulkanPipeline::getPipeline() const {
        return mPipeline;
    }

    const vk::PipelineLayout &VulkanPipeline::getPipelineLayout() const {
        return mPipelineLayout;
    }
} // engine