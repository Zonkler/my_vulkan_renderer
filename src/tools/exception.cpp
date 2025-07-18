#include "tools/exception.hpp"
#include "tools/VulkanTools.hpp"
#include <string>
VulkanException::VulkanException(std::string message, const VkResult result)
    : MagmaException(message.append(" (")
                          .append(vkutils::as_string(result))
                          .append(": ")
                          .append(vkutils::result_to_description(result))
                          .append(")")) {}