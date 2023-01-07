#include "Engine.h"

#include <iostream>
#include <format>
#include <map>
#include <set>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <spdlog/spdlog.h>

namespace engine
{
	/**
	* Initialize external libraries
	*/
	Engine::Engine()
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
	Engine::~Engine()
	{
		spdlog::debug(std::format("destroying engine resources"));
		destroyInstance();
		SDL_Quit();
	}

	/**
	* create Vulkan instance
	*/
	void Engine::createInstance()
	{
		if (_enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available");
		}

		std::vector<const char*> extensions;
		try
		{
			extensions = getRequiredExtensions();
		}
		catch (const std::exception& e)
		{
			throw e;
		}

		/* Create vulkan instance */
		VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "vulkan_hello",
			.applicationVersion = 0,
			.pEngineName = "vulkan_engine",
			.engineVersion = 0,
			.apiVersion = VK_API_VERSION_1_3
		};

		VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (_enableValidationLayers)
		{
			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCallback;
			createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
			createInfo.ppEnabledLayerNames = _validationLayers.data();
		}
		else
		{
			createInfo.pNext = nullptr;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
		if (result != VkResult::VK_SUCCESS)
		{
			throw::std::runtime_error(std::format("Failed to create Vulkan instance"));
		}
	}

	/**
	* setting up validation layer's debug messenger
	*/
	void Engine::setupDebugMessenger()
	{
		if (!_enableValidationLayers)
		{
			return;
		}

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessaenger) != VK_SUCCESS)
		{
			throw new std::runtime_error(std::format("failed to set a debug messenger"));
		}
	}

	/**
	* enumerate Vulkan extensions
	*/
	void Engine::searchExtensions()
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
	* create VkSurface
	*/
	void Engine::createSurface()
	{
		if (SDL_Vulkan_CreateSurface(_window, _instance, &_surface) != SDL_TRUE) {
			throw std::runtime_error(std::format("failed to create VkSurface: {}", SDL_GetError()));
		}
	}

	/**
	* select physical device(grpahics card)
	*/
	void Engine::selectPhysicalDevice()
	{
		/* Physical device initialization */
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
		if (deviceCount == 0)
		{
			throw new std::runtime_error(std::format("failed to find GPUs with vulkan support"));
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

		_physicalDevice = pickSuitablePhysicalDevice(devices);
		if (_physicalDevice == VK_NULL_HANDLE)
		{
			throw new std::runtime_error(std::format("failed to find suitable GPU"));
		}
	}

	/**
	* create logical device
	*/
	void Engine::createLogicalDevice()
	{
		QueueFamilyIndicies indices = findQueueFamilyIndices(_physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = queueFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority,
			};

			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size()),
			.ppEnabledExtensionNames = _deviceExtensions.data(),
			.pEnabledFeatures = &deviceFeatures,
		};

		if (_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
			createInfo.ppEnabledLayerNames = _validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
		{
			throw std::runtime_error(std::format("failed to create logical device"));
		}

		vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
	}

	/**
	* check validation layer support
	*/
	bool Engine::checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : _validationLayers)
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
	VkResult Engine::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
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
	void Engine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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
	void Engine::destroyDebugUtilsmessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func == nullptr)
		{
			return;
		}

		func(instance, _debugMessaenger, pAllocator);
	}

	/**
	* getting required vulkan extensions
	*/
	std::vector<const char*> Engine::getRequiredExtensions()
	{
		uint32_t sdlExtensionCount = 0;
		if (SDL_Vulkan_GetInstanceExtensions(_window, &sdlExtensionCount, nullptr) != SDL_TRUE)
		{
			throw std::runtime_error(std::format("failed to get SDL required extensions, {}", SDL_GetError()));
		}

		std::vector<const char*> sdlExtensions(sdlExtensionCount);
		if (SDL_Vulkan_GetInstanceExtensions(_window, &sdlExtensionCount, sdlExtensions.data()) != SDL_TRUE)
		{
			throw std::runtime_error(std::format("failed to get SDL required extensions, {}", SDL_GetError()));
		}

		if (_enableValidationLayers)
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
	VkPhysicalDevice Engine::pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices)
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
	int Engine::calculatePhysicalDeviceScore(const VkPhysicalDevice& device)
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
	bool Engine::isDeviceSuitable(const VkPhysicalDevice& device)
	{
		QueueFamilyIndicies indices = findQueueFamilyIndices(device);

		bool extensionSupported = checkDeviceExtensionSupport(device);

		return indices.isComplete() && extensionSupported;
	}

	/**
	* check physical device supports extension
	*/
	bool Engine::checkDeviceExtensionSupport(const VkPhysicalDevice& device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

		for (const VkExtensionProperties& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	/**
	* find queue-family from physical device
	*/
	QueueFamilyIndicies Engine::findQueueFamilyIndices(const VkPhysicalDevice& device)
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

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

			if (presentSupport)
			{
				indicies.presentFamily = i;
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
	void Engine::destroyInstance()
	{
		vkDestroyDevice(_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		destroyDebugUtilsmessengerEXT(_instance, _debugMessaenger, nullptr);
		vkDestroyInstance(_instance, nullptr);
	}
}