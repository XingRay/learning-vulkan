//
// Created by leixing on 2025/1/5.
//

#include "VulkanShader.h"
#include "engine/Log.h"

#include "engine/VulkanUtil.h"
#include "engine/vulkan_wrapper/VulkanUniformBuffer.h"
#include "engine/vulkan_wrapper/VulkanSamplerBuffer.h"

namespace engine {

    VulkanShader::VulkanShader(const VulkanDevice &vulkanDevice,
                               const VulkanCommandPool &commandPool,
                               uint32_t frameCount,
                               const std::vector<char> &vertexShaderCode,
                               const std::vector<char> &fragmentShaderCode,
                               const std::vector<VulkanVertex> &vertices,
                               const std::vector<VulkanDescriptorSet> &descriptorSets,
                               const std::vector<VulkanPushConstant> &pushConstants)
            : mVulkanDevice(vulkanDevice) {

        mVertexShaderModule = vulkanDevice.createShaderModule(vertexShaderCode);
        mFragmentShaderModule = vulkanDevice.createShaderModule(fragmentShaderCode);

        for (const VulkanVertex &vertex: vertices) {
            vk::VertexInputBindingDescription bindingDescription{};
            bindingDescription
                    .setBinding(vertex.binding)
                    .setStride(vertex.size)
                    .setInputRate(vk::VertexInputRate::eVertex);
            mVertexDescriptions.push_back(bindingDescription);

            for (const VulkanVertexAttribute &attribute: vertex.attributes) {
                vk::VertexInputAttributeDescription attributeDescription{};
                attributeDescription
                        .setLocation(attribute.location)
                        .setFormat(attribute.format)
                        .setOffset(attribute.offset)
                        .setBinding(attribute.binding);

                mVertexInputAttributeDescriptions.push_back(attributeDescription);
            }
        }


        // 统计各种类型的描述符(uniform, sampler, storage ...)出现的次数
        std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
        for (const VulkanDescriptorSet &descriptorSet: descriptorSets) {

            std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
            descriptorSetLayoutBindings.reserve(descriptorSet.descriptors.size());

            for (const VulkanDescriptor &descriptor: descriptorSet.descriptors) {
                vk::DescriptorPoolSize &descriptorPoolSize = VulkanUtil::getOrCreateDescriptorPoolSize(descriptorPoolSizes, descriptor.getDescriptorType());
                // 每一帧中各种类型的描述符(uniform/sampler/storage ...)出现的次数还需要乘以帧数得到总共需要的描述符的数量
                descriptorPoolSize.descriptorCount += descriptor.getDescriptorCount() * frameCount;

                vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding{};
                descriptorSetLayoutBinding
                        .setBinding(descriptor.getBinding())
                        .setDescriptorType(descriptor.getDescriptorType())
                        .setDescriptorCount(descriptor.getDescriptorCount())
                        .setStageFlags(descriptor.getStageFlags())
                        .setPImmutableSamplers(nullptr);

                descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);
            }

            vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
            descriptorSetLayoutCreateInfo
                    .setBindings(descriptorSetLayoutBindings);

            vk::DescriptorSetLayout descriptorSetLayout = mVulkanDevice.getDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
            mDescriptorSetLayouts.push_back(descriptorSetLayout);
        }

        vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo
                .setPoolSizes(descriptorPoolSizes)
                        // 设置描述符池可以分配的最大描述符集(UniformSet)的数量
                .setMaxSets(frameCount * descriptorSets.size())
                .setFlags(vk::DescriptorPoolCreateFlags{});

        LOG_D("create DescriptorPool");
        mDescriptorPool = mVulkanDevice.getDevice().createDescriptorPool(descriptorPoolCreateInfo);

        LOG_D("allocate DescriptorSets");
        vk::DescriptorSetAllocateInfo allocateInfo{};
        allocateInfo
                .setDescriptorPool(mDescriptorPool)
                .setSetLayouts(mDescriptorSetLayouts);

        // mDescriptorSets 的大小为 frameCount * descriptorSetLayouts.size()。
        mDescriptorSets.resize(frameCount);
        for (uint32_t i = 0; i < frameCount; i++) {
            std::vector<vk::DescriptorSet> sets = mVulkanDevice.getDevice().allocateDescriptorSets(allocateInfo);
            mDescriptorSets[i] = std::move(sets);
        }

        // create buffers
        mBuffers.reserve(frameCount);
        for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
            std::vector<std::vector<std::unique_ptr<VulkanBuffer>>> buffersOfFrame;
            buffersOfFrame.reserve(descriptorSets.size());

            for (const VulkanDescriptorSet &set: descriptorSets) {
                std::vector<std::unique_ptr<VulkanBuffer>> buffersOfSet;
                buffersOfSet.reserve(set.descriptors.size());

                for (const VulkanDescriptor &descriptor: set.descriptors) {
                    vk::DescriptorType type = descriptor.getDescriptorType();

                    if (type == vk::DescriptorType::eUniformBuffer) {
                        vk::DeviceSize uniformSize = descriptor.getUniformData().size;
                        buffersOfSet.push_back(std::make_unique<VulkanUniformBuffer>(vulkanDevice, uniformSize, descriptor.getBinding(), descriptor.getIndex()));
                    } else if (type == vk::DescriptorType::eCombinedImageSampler) {
                        const ImageSize &imageSize = descriptor.getSamplerData().imageSize;
                        buffersOfSet.push_back(std::make_unique<VulkanSamplerBuffer>(vulkanDevice, commandPool,
                                                                                     imageSize.width, imageSize.height, imageSize.channels,
                                                                                     descriptor.getBinding(), descriptor.getIndex()));
                    } else {
                        throw std::runtime_error("unsupported type of descriptor");
                    }

                }
                buffersOfFrame.push_back(std::move(buffersOfSet));
            }
            mBuffers.push_back(std::move(buffersOfFrame));
        }

        // updateDescriptorSets
        std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
        for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
            for (int setIndex = 0; setIndex < descriptorSets.size(); setIndex++) {
                const VulkanDescriptorSet &set = descriptorSets[setIndex];
                for (int descriptorIndex = 0; descriptorIndex < set.descriptors.size(); descriptorIndex++) {
                    const VulkanDescriptor &descriptor = set.descriptors[descriptorIndex];
                    vk::DescriptorType type = descriptor.getDescriptorType();
                    vk::WriteDescriptorSet writeDescriptorSet{};

                    if (type == vk::DescriptorType::eUniformBuffer) {
                        VulkanUniformBuffer *pUniformBuffer = dynamic_cast<VulkanUniformBuffer *>(mBuffers[frameIndex][setIndex][descriptorIndex].get());
                        vk::DescriptorBufferInfo descriptorBufferInfo{};
                        descriptorBufferInfo
                                .setBuffer(pUniformBuffer->getUniformBuffer())
                                .setOffset(pUniformBuffer->getOffset())
                                .setRange(pUniformBuffer->getBufferSize());

                        std::array<vk::DescriptorBufferInfo, 1> descriptorBufferInfos = {descriptorBufferInfo};

                        writeDescriptorSet
                                .setDstSet(mDescriptorSets[frameIndex][setIndex])
                                .setDstBinding(pUniformBuffer->getBinding())
                                .setDstArrayElement(pUniformBuffer->getIndex())
                                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                                .setBufferInfo(descriptorBufferInfos);

                    } else if (type == vk::DescriptorType::eCombinedImageSampler) {
                        VulkanSamplerBuffer *pSamplerBuffer = dynamic_cast<VulkanSamplerBuffer *>(mBuffers[frameIndex][setIndex][descriptorIndex].get());
                        vk::DescriptorImageInfo samplerDescriptorImageInfo;
                        samplerDescriptorImageInfo
                                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                                .setImageView(pSamplerBuffer->getTextureImageView())
                                .setSampler(pSamplerBuffer->getTextureSampler());

                        std::array<vk::DescriptorImageInfo, 1> samplerDescriptorImageInfos = {samplerDescriptorImageInfo};

                        writeDescriptorSet
                                .setDstSet(mDescriptorSets[frameIndex][setIndex])
                                .setDstBinding(pSamplerBuffer->getBinding())
                                .setDstArrayElement(pSamplerBuffer->getIndex())
                                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                                .setImageInfo(samplerDescriptorImageInfos);

                    } else {
                        throw std::runtime_error("unsupported type of descriptor");
                    }

                    writeDescriptorSets.push_back(writeDescriptorSet);
                }
            }
        }
        vulkanDevice.getDevice().updateDescriptorSets(writeDescriptorSets, nullptr);


        // push constant
        vk::PhysicalDeviceProperties deviceProperties = vulkanDevice.getPhysicalDevice().getProperties();
        uint32_t maxPushConstantsSize = deviceProperties.limits.maxPushConstantsSize;
        uint32_t totalPushConstantSize = 0;
        for (const VulkanPushConstant &pushConstant: pushConstants) {
            // 检查单个 Push Constant 的大小是否合理
            if (pushConstant.size == 0) {
                throw std::runtime_error("Push Constant size cannot be zero");
            }

            // 检查偏移和大小是否超出设备限制
            if (pushConstant.offset + pushConstant.size > maxPushConstantsSize) {
                throw std::runtime_error(
                        "Push Constant range (offset: " + std::to_string(pushConstant.offset) +
                        ", size: " + std::to_string(pushConstant.size) +
                        ") exceeds device limit: " + std::to_string(maxPushConstantsSize)
                );
            }

            vk::PushConstantRange pushConstantRange{};
            pushConstantRange
                    .setStageFlags(pushConstant.stageFlagBits)
                    .setOffset(pushConstant.offset)
                    .setSize(pushConstant.size);
            mPushConstantRanges.push_back(pushConstantRange);

            // 创建数据缓冲区
            std::vector<uint8_t> data(pushConstant.size);
            mPushConstantDataList.push_back(std::move(data));

            totalPushConstantSize += pushConstant.size;
        }

        // 检查总大小是否超出设备限制
        if (totalPushConstantSize > maxPushConstantsSize) {
            throw std::runtime_error(
                    "Total Push Constant size (" + std::to_string(totalPushConstantSize) +
                    ") exceeds device limit: " + std::to_string(maxPushConstantsSize)
            );
        }
    }

    VulkanShader::~VulkanShader() {
        LOG_D("VulkanDescriptorSet::~VulkanDescriptorSet");
        const vk::Device device = mVulkanDevice.getDevice();

        device.destroy(mVertexShaderModule);
        device.destroy(mFragmentShaderModule);

        device.destroyDescriptorPool(mDescriptorPool);

        for (const auto &descriptorSetLayout: mDescriptorSetLayouts) {
            device.destroyDescriptorSetLayout(descriptorSetLayout);
        }
    }

    /**
     * shader code
     */
    const vk::ShaderModule &VulkanShader::getVertexShaderModule() const {
        return mVertexShaderModule;
    }

    const vk::ShaderModule &VulkanShader::getFragmentShaderModule() const {
        return mFragmentShaderModule;
    }

    const std::vector<vk::VertexInputBindingDescription> &VulkanShader::getVertexDescriptions() const {
        return mVertexDescriptions;
    }

    const std::vector<vk::VertexInputAttributeDescription> &VulkanShader::getVertexInputAttributeDescriptions() const {
        return mVertexInputAttributeDescriptions;
    }

    const std::vector<vk::DescriptorSetLayout> &VulkanShader::getDescriptorSetLayouts() const {
        return mDescriptorSetLayouts;
    }

    const std::vector<vk::DescriptorSet> &VulkanShader::getDescriptorSets(uint32_t frameIndex) const {
        return mDescriptorSets[frameIndex];
    }


    const std::vector<vk::PushConstantRange> &VulkanShader::getPushConstantRanges() const {
        return mPushConstantRanges;
    }

    const std::vector<std::vector<uint8_t>> &VulkanShader::getPushConstantDataList() const {
        return mPushConstantDataList;
    }

    void VulkanShader::updateBuffer(uint32_t frameIndex, uint32_t set, uint32_t binding, void *data, uint32_t size) {
        mBuffers[frameIndex][set][binding]->updateBuffer(data, size);
    }

    void VulkanShader::updatePushConstant(uint32_t index, const void *data) {
        std::memcpy(mPushConstantDataList[index].data(), data, mPushConstantDataList[index].size());
    }
} // engine