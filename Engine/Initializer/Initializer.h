#ifndef _ENGINE_INITIALIZER_HEADER_
#define _ENGINE_INITIALIZER_HEADER_

#include <vector>
#include <optional>

#include <SDL3/SDL.h>

#include <vulkan/vulkan.h>

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

	void createInstance(SDL_Window* window);
	void getExtensions();
	void selectPhysicalDevice();
	void createLogicalDevice();
	void destroyInstance();

	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions(SDL_Window* window);
	VkPhysicalDevice pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
	int calculatePhysicalDeviceScore(const VkPhysicalDevice& device);
	bool isDeviceSuitable(const VkPhysicalDevice& device);
	QueueFamilyIndicies findQueueFamilyIndices(const VkPhysicalDevice& device);

}

#endif