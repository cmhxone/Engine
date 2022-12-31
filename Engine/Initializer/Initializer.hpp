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
	namespace vulkan
	{
		VkInstance instance;

		/**
		* create Vulkan instance
		*/
		void createInstance()
		{
			/* Create vulkan instance */
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
		* enumerate Vulkan extensions
		*/
		void getExtensions()
		{
			/* Log Vulkan extensions */
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			for (const VkExtensionProperties& extension : extensions)
			{
				spdlog::debug(std::format("Vulkan extensions: {}", extension.extensionName));
			}
		}

		/**
		* select physical device(grpahics card)
		*/
		void selectPhysicalDevice()
		{
			/* Physical device initialization */
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
			if (deviceCount == 0)
			{
				throw new std::runtime_error(std::format("failed to find GPUs with vulkan support"));
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			std::multimap<int, VkPhysicalDevice> candidates;

			for (const VkPhysicalDevice& device : devices)
			{
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(device, &deviceProperties);
				VkPhysicalDeviceFeatures deviceFeatures;
				vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

				int score = 0;
				score += deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1'000 : 0;
				score += deviceProperties.limits.maxImageDimension2D;
				score = !deviceFeatures.geometryShader ? 0 : score;

				spdlog::debug(std::format("Physical device Info: name={}, score={}", deviceProperties.deviceName, score));

				candidates.insert(std::make_pair(score, device));
			}

			if (candidates.rbegin()->first > 0)
			{
				physicalDevice = candidates.rbegin()->second;
			}
			else
			{
				throw std::runtime_error(std::format("failed to find suitable GPU"));
			}

			if (physicalDevice == VK_NULL_HANDLE)
			{
				throw new std::runtime_error(std::format("failed to find suitable GPU"));
			}
		}

		/**
		* destroy Vulkan instance
		*/
		void destroyInstance()
		{
			vkDestroyInstance(instance, nullptr);
		}
	}

	/**
	* Initialize external libraries
	*/
	void initialize()
	{
		/* Spdlog configuration */
		spdlog::set_level(spdlog::level::debug);

		/* SDL Initialization */
		spdlog::debug(std::format("initializing engine resources"));
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			throw std::runtime_error(std::format("Failed to initialize SDL library, {}", SDL_GetError()));
		}

		try
		{
			vulkan::createInstance();
			vulkan::getExtensions();
			vulkan::selectPhysicalDevice();
		}
		catch (const std::exception& e)
		{
			throw e;
		}
	}

	/**
	* Destroy external libraries
	*/
	void destroy()
	{
		spdlog::debug(std::format("destroying engine resources"));
		vulkan::destroyInstance();
		SDL_Quit();
	}
}

#endif