#include "Initializer.h"

#include <iostream>
#include <format>
#include <map>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <spdlog/spdlog.h>

namespace engine
{
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

namespace engine::vulkan
{
	VkInstance instance = nullptr;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifndef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif // !NDEBUG


	/**
	* create Vulkan instance
	*/
	void createInstance(SDL_Window* window)
	{
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available");
		}

		std::vector<const char*> extensions;
		try
		{
			extensions = getRequiredExtensions(window);
		}
		catch (const std::exception& e)
		{
			throw e;
		}

		/* Create vulkan instance */
		const VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "vulkan_hello",
			.applicationVersion = 0,
			.pEngineName = "vulkan_engine",
			.engineVersion = 0,
			.apiVersion = VK_API_VERSION_1_3
		};

		const VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0,
			.ppEnabledLayerNames = enableValidationLayers ? validationLayers.data() : nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
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
			spdlog::debug(std::format("Loaded extension: {}", extension.extensionName));
		}
	}

	/**
	* select physical device(grpahics card)
	*/
	void selectPhysicalDevice()
	{
		/* Physical device initialization */
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0)
		{
			throw new std::runtime_error(std::format("failed to find GPUs with vulkan support"));
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		physicalDevice = pickSuitablePhysicalDevice(devices);
		if (physicalDevice == VK_NULL_HANDLE)
		{
			throw new std::runtime_error(std::format("failed to find suitable GPU"));
		}
	}

	void createLogicalDevice()
	{
		QueueFamilyIndicies indices = findQueueFamilyIndices(physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = indices.graphicsFamily.value(),
			.queueCount = 1
		};

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &queueCreateInfo,
			.enabledExtensionCount = 0,
			.pEnabledFeatures = &deviceFeatures,
		};
	}

	bool checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const VkLayerProperties& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> getRequiredExtensions(SDL_Window* window)
	{
		uint32_t sdlExtensionCount = 0;
		if (SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr) != SDL_TRUE)
		{
			throw std::runtime_error(std::format("failed to get SDL required extensions, {}", SDL_GetError()));
		}

		std::vector<const char*> sdlExtensions(sdlExtensionCount);
		if (SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, sdlExtensions.data()) != SDL_TRUE)
		{
			throw std::runtime_error(std::format("failed to get SDL required extensions, {}", SDL_GetError()));
		}

		if (enableValidationLayers)
		{
			sdlExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		for (const char* extension : sdlExtensions)
		{
			spdlog::debug(std::format("Required extension: {}", extension));
		}

		return sdlExtensions;
	}

	VkPhysicalDevice pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices)
	{
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const VkPhysicalDevice& device : devices)
		{
			candidates.insert(std::make_pair(calculatePhysicalDeviceScore(device), device));
		}

		if (candidates.rbegin()->first > 0)
		{
			return candidates.rbegin()->second;
		}
		else
		{
			return VK_NULL_HANDLE;
		}
	}

	/* calculate physical devices processing score */
	int calculatePhysicalDeviceScore(const VkPhysicalDevice& device)
	{
		int score = 0;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		score += deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1'000 : 0;
		score += deviceProperties.limits.maxImageDimension2D;
		score = !deviceFeatures.geometryShader ? 0 : score;

		spdlog::debug(std::format("Physical device Info: name={}, score={}", deviceProperties.deviceName, score));

		return score;
	}

	/**
	* check if physical device is suitable
	*/
	bool isDeviceSuitable(const VkPhysicalDevice& device)
	{
		QueueFamilyIndicies indices = findQueueFamilyIndices(device);

		return indices.isComplete();
	}

	/**
	* find queue-family from physical device
	*/
	QueueFamilyIndicies findQueueFamilyIndices(const VkPhysicalDevice& device)
	{
		QueueFamilyIndicies indicies;

 		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indicies.graphicsFamily = i;
			}

			if (indicies.isComplete()) {
				break;
			}

			i++;
		}

		return indicies;
	}

	/**
	* destroy Vulkan instance
	*/
	void destroyInstance()
	{
		vkDestroyInstance(instance, nullptr);
	}
}