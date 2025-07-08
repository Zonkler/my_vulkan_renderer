#include <string>

namespace vkutils
{
std::string_view as_string(VkResult result);
std::string_view result_to_description(VkResult result);

void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& cmd);


} // namespace vkutils