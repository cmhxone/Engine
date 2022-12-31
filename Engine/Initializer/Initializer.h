#ifndef _ENGINE_INITIALIZER_HEADER_
#define _ENGINE_INITIALIZER_HEADER_

#include <iostream>
#include <format>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <spdlog/spdlog.h>

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