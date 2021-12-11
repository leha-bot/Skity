#ifndef SKITY_SRC_RENDER_HW_VK_VK_PIPELINE_WRAPPER_HPP
#define SKITY_SRC_RENDER_HW_VK_VK_PIPELINE_WRAPPER_HPP

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <memory>
#include <skity/gpu/gpu_context.hpp>
#include <vector>

namespace skity {

struct GlobalPushConst {
  alignas(16) glm::mat4 mvp = {};
  alignas(16) int32_t premul_alpha = {};
};

struct ColorInfoSet {
  alignas(16) float global_alpha = 1.f;
  alignas(16) glm::vec4 user_color = {};
};

class VKPipelineWrapper {
 public:
  VKPipelineWrapper(size_t push_const_size)
      : push_const_size_(push_const_size) {}
  virtual ~VKPipelineWrapper() = default;

  void Init(GPUVkContext* ctx, VkShaderModule vertex, VkShaderModule fragment);

  void Destroy(GPUVkContext* ctx);

  void Bind(VkCommandBuffer cmd);

  static std::unique_ptr<VKPipelineWrapper> CreateStaticColorPipeline(
      GPUVkContext* ctx);

 protected:
  void InitDescriptorSetLayout(GPUVkContext* ctx);
  void InitPipelineLayout(GPUVkContext* ctx);

  virtual std::vector<VkDescriptorSetLayout> GenearteDescriptorSetLayout(
      GPUVkContext* ctx);

  virtual VkPipelineDepthStencilStateCreateInfo
  GetDepthStencilStateCreateInfo() = 0;

  virtual VkPipelineColorBlendAttachmentState GetColorBlendState();

  virtual std::vector<VkDynamicState> GetDynamicStates();

 private:
  VkVertexInputBindingDescription GetVertexInputBinding();
  std::array<VkVertexInputAttributeDescription, 2> GetVertexInputAttributes();

 private:
  size_t push_const_size_;
  std::vector<VkDescriptorSetLayout> descriptor_set_layout_ = {};
  VkPipelineLayout pipeline_layout_ = {};
  VkPipeline pipeline_ = {};
};

}  // namespace skity

#endif  // SKITY_SRC_RENDER_HW_VK_VK_PIPELINE_WRAPPER_HPP