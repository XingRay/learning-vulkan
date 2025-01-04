//
// Created by leixing on 2024/12/28.
//

#include "VulkanDescriptorSet.h"
#include "Log.h"

namespace engine {
    VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDevice &device, uint32_t frameCount/*,
                                             const std::vector<vk::Buffer> &transformUniformBuffers, uint32_t transformUniformBufferSize,
                                             const std::vector<vk::Buffer> &colorUniformBuffers, uint32_t colorUniformBufferSize*/
    ) : mDevice(device) {
        LOG_D("VulkanDescriptorSet");
//        LOG_D("transformUniformBufferSize: %d", transformUniformBufferSize);
//        LOG_D("colorUniformBufferSize: %d", colorUniformBufferSize);

        std::vector<vk::DescriptorPoolSize> poolSizes;

        // createDescriptorPool
        if (false) {
            vk::DescriptorPoolSize transformUboPoolSize{};
            transformUboPoolSize.setType(vk::DescriptorType::eUniformBuffer)
                    .setDescriptorCount(frameCount);

            poolSizes.push_back(transformUboPoolSize);
        }


        if (false) {
            vk::DescriptorPoolSize colorUboPoolSize{};
            colorUboPoolSize.setType(vk::DescriptorType::eUniformBuffer)
                    .setDescriptorCount(frameCount);
            poolSizes.push_back(colorUboPoolSize);
        }


        vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo
                .setPoolSizes(poolSizes)
                .setMaxSets(frameCount)
//            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
//                .setFlags(vk::DescriptorPoolCreateFlags{});
                .setFlags(static_cast<vk::DescriptorPoolCreateFlags>(0));

        mDescriptorPool = mDevice.getDevice().createDescriptorPool(descriptorPoolCreateInfo);


        // createDescriptorSetLayout
        vk::DescriptorSetLayoutBinding transformUboLayoutBinding{};
        transformUboLayoutBinding
                .setBinding(0)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                .setStageFlags(vk::ShaderStageFlagBits::eVertex)
                .setPImmutableSamplers(nullptr);

        vk::DescriptorSetLayoutBinding colorUboLayoutBinding{};
        colorUboLayoutBinding
                .setBinding(1)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                .setStageFlags(vk::ShaderStageFlagBits::eVertex)
                .setPImmutableSamplers(nullptr);

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {transformUboLayoutBinding, colorUboLayoutBinding};


        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
//        descriptorSetLayoutCreateInfo
//                .setBindings(bindings);

        mDescriptorSetLayout = mDevice.getDevice().createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

        // createDescriptorSets
        // 大小为 imageCount，并且每个元素都初始化为 mDescriptorSetLayout
        std::vector<vk::DescriptorSetLayout> layouts(frameCount, mDescriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.setDescriptorPool(mDescriptorPool)
                .setSetLayouts(layouts);

        mDescriptorSets = mDevice.getDevice().allocateDescriptorSets(allocateInfo);

//        for (int i = 0; i < frameCount; i++) {
//            // 描述符绑定到 TransformUniformBufferObject
//            vk::DescriptorBufferInfo transformUboDescriptorBufferInfo{};
//            transformUboDescriptorBufferInfo.setBuffer(transformUniformBuffers[i]) // TransformUniformBuffer
//                    .setOffset(0)
////                    .setRange(vk::WholeSize)
//                    .setRange(transformUniformBufferSize);
//
//            vk::WriteDescriptorSet transformUboWriteDescriptorSet{};
//            transformUboWriteDescriptorSet.setDstSet(mDescriptorSets[i])
//                    .setDstBinding(0) // 对应 binding = 0
//                    .setDstArrayElement(0)
//                    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
//                    .setDescriptorCount(1)
//                    .setPBufferInfo(&transformUboDescriptorBufferInfo)
//                    .setPImageInfo(nullptr)
//                    .setPTexelBufferView(nullptr);
//
//            // 描述符绑定到 ColorUniformBufferObject
//            vk::DescriptorBufferInfo colorUboDescriptorBufferInfo{};
//            colorUboDescriptorBufferInfo.setBuffer(colorUniformBuffers[i]) // ColorUniformBuffer
//                    .setOffset(0)
////                    .setRange(vk::WholeSize);
//                    .setRange(colorUniformBufferSize);
//
//            vk::WriteDescriptorSet colorUboWriteDescriptorSet{};
//            colorUboWriteDescriptorSet.setDstSet(mDescriptorSets[i])
//                    .setDstBinding(1) // 对应 binding = 1
//                    .setDstArrayElement(0)
//                    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
//                    .setDescriptorCount(1)
//                    .setPBufferInfo(&colorUboDescriptorBufferInfo)
//                    .setPImageInfo(nullptr)
//                    .setPTexelBufferView(nullptr);
//
//            // 将两个 WriteDescriptorSet 添加到数组
//            std::array<vk::WriteDescriptorSet, 2> writeDescriptorSets = {
//                    transformUboWriteDescriptorSet,
//                    colorUboWriteDescriptorSet
//            };
//
//            // 更新描述符集
//            mDevice.getDevice().updateDescriptorSets(writeDescriptorSets, nullptr);
//        }
    }

    VulkanDescriptorSet::~VulkanDescriptorSet() {
        LOG_D("VulkanDescriptorSet::~VulkanDescriptorSet");
        const vk::Device device = mDevice.getDevice();
        device.destroyDescriptorPool(mDescriptorPool);
        device.destroyDescriptorSetLayout(mDescriptorSetLayout);
    }

    const vk::DescriptorSetLayout &VulkanDescriptorSet::getDescriptorSetLayout() const {
        return mDescriptorSetLayout;
    }

    const vk::DescriptorPool &VulkanDescriptorSet::getDescriptorPool() const {
        return mDescriptorPool;
    }

    const std::vector<vk::DescriptorSet> &VulkanDescriptorSet::getDescriptorSets() const {
        return mDescriptorSets;
    }
}
