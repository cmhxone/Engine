#ifndef _ENGINE_INITIALIZER_HEADER_
#define _ENGINE_INITIALIZER_HEADER_

#include <vector>
#include <optional>
#include <format>

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

namespace engine
{
	void initialize();
	void destroy();
}

namespace engine::vulkan
{
	struct QueueFamilyIndicies
	{
		std::optional<uint32_t> graphicsFamily;

		constexpr const bool isComplete() const
		{
			return graphicsFamily.has_value();
		}
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		if (messageServerity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			return VK_FALSE;
		}

		spdlog::error(std::format("validation layer: {}", pCallbackData->pMessage));

		return VK_FALSE;
	}

	void createInstance(SDL_Window* window);
	void setupDebugMessenger();
	void getExtensions();
	void selectPhysicalDevice();
	void createLogicalDevice();
	void destroyInstance();

	bool checkValidationLayerSupport();
	VkResult createDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger
	);
	void destroyDebugUtilsmessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	std::vector<const char*> getRequiredExtensions(SDL_Window* window);
	VkPhysicalDevice pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
	int calculatePhysicalDeviceScore(const VkPhysicalDevice& device);
	bool isDeviceSuitable(const VkPhysicalDevice& device);
	QueueFamilyIndicies findQueueFamilyIndices(const VkPhysicalDevice& device);

}

#endif