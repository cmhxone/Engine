#ifndef _ENGINE_INITIALIZER_HEADER_
#define _ENGINE_INITIALIZER_HEADER_

#include <vector>
#include <optional>

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

	void createInstance();
	void getExtensions();
	void selectPhysicalDevice();
	void destroyInstance();

	VkPhysicalDevice pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
	int calculatePhysicalDeviceScore(const VkPhysicalDevice& device);
	bool isDeviceSuitable(const VkPhysicalDevice& device);
	QueueFamilyIndicies findQueueFamilyIndices(const VkPhysicalDevice& device);

}

#endif