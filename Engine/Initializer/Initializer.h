#ifndef _ENGINE_INITIALIZER_HEADER_
#define _ENGINE_INITIALIZER_HEADER_

#include <vector>
#include <vulkan/vulkan.h>

namespace engine
{
	void initialize();
	void destroy();
}

namespace engine::vulkan
{
	void createInstance();
	void getExtensions();
	void selectPhysicalDevice();
	void destroyInstance();

	VkPhysicalDevice pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
	int calculatePhysicalDeviceScore(const VkPhysicalDevice& device);
}

#endif