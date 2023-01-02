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
	VkDebugUtilsMessengerEXT debugMessaenger = nullptr;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;

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

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		populateDebugMessengerCreateInfo(debugCreateInfo);

		const VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = enableValidationLayers ? (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo : nullptr,
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
	* setting up validation layer's debug messenger
	*/
	void setupDebugMessenger()
	{
		if (!enableValidationLayers)
		{
			return;
		}

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessaenger) != VK_SUCCESS)
		{
			throw new std::runtime_error(std::format("failed to set a debug messenger"));
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

	/**
	* create logical device
	*/
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

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		{
			throw std::runtime_error(std::format("failed to create logical device"));
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	}

	/**
	* check validation layer support
	*/
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

	/**
	* create debug utils messenger extension
	*/
	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func == nullptr)
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}

	/**
	* popluate debug messenger create-info
	*/
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity
				= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType
				= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
			.pfnUserCallback = debugCallback
		};
	}

	/**
	* destroy debug utils messenger extension
	*/
	void destroyDebugUtilsmessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func == nullptr)
		{
			return;
		}

		func(instance, debugMessaenger, pAllocator);
	}

	/**
	* getting required vulkan extensions
	*/
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

	/**
	* pick the most suitable physical device
	*/
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

	/**
	* calculate physical devices processing score
	*/
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
		vkDestroyDevice(device, nullptr);
		destroyDebugUtilsmessengerEXT(instance, debugMessaenger, nullptr);
		vkDestroyInstance(instance, nullptr);
	}
}