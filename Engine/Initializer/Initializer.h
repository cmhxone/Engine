#ifndef _ENGINE_INITIALIZER_HEADER_
#define _ENGINE_INITIALIZER_HEADER_

#include <iostream>
#include <format>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <spdlog/spdlog.h>

namespace engine
{
	/**
	* Initialize external libraries
	*/
	void initialize(VkInstance instance)
	{
		spdlog::set_level(spdlog::level::debug);

		spdlog::debug(std::format("initializing engine resources"));
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			throw std::runtime_error(std::format("Failed to initialize SDL library, {}", SDL_GetError()));
		}

		const VkApplicationInfo appinfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "vulkan_hello",
		.applicationVersion = 0,
		.pEngineName = "vulkan_engine",
		.engineVersion = 0,
		.apiVersion = VK_API_VERSION_1_3
		};

		const VkInstanceCreateInfo createinfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appinfo,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = 0,
			.ppEnabledExtensionNames = nullptr
		};

		VkResult result = vkCreateInstance(&createinfo, nullptr, &instance);
		if (result != VkResult::VK_SUCCESS)
		{
			throw::std::runtime_error(std::format("Failed to create Vulkan instance"));
		}
	}

	/**
	* Destroy external libraries
	*/
	void destroy(VkInstance instance)
	{
		spdlog::debug(std::format("destroying engine resources"));
		vkDestroyInstance(instance, nullptr);
		SDL_Quit();
	}
}

#endif