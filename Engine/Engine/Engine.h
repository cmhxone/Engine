#ifndef _ENGINE_INITIALIZER_HEADER_
#define _ENGINE_INITIALIZER_HEADER_

#include <vector>
#include <optional>
#include <format>

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "../Prototype/Singleton.hpp"

namespace engine
{
	/**
	* Vulkan queue family indicies
	*/
	struct QueueFamilyIndicies
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		constexpr const bool isComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	/**
	* Vulkan swap chain support details
	*/
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class Engine : public Singleton<Engine>
	{
	public:
		Engine();
		~Engine();

		void setSDLWindow(SDL_Window* window) { _window = window; };

		void createInstance();
		void setupDebugMessenger();
		void searchExtensions();
		void createSurface();
		void selectPhysicalDevice();
		void createLogicalDevice();
		void createSwapChain();
		void destroyInstance();

		constexpr const VkInstance getVkInstance() const { return _instance; }
		constexpr const VkSurfaceKHR getVkSurface() const { return _surface; }

		constexpr const SDL_Window* getSDLWindow() const { return _window; }

	protected:

	private:
		VkInstance _instance = nullptr;
		VkDebugUtilsMessengerEXT _debugMessaenger = nullptr;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkQueue _graphicsQueue = VK_NULL_HANDLE;
		VkQueue _presentQueue = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> _swapChainImages;
		VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent;

		SDL_Window* _window;

		const std::vector<const char*> _validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> _deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

#ifndef NDEBUG
		const bool _enableValidationLayers = false;
#else
		const bool _enableValidationLayers = true;
#endif // !NDEBUG

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		)
		{
			spdlog::error(std::format("validation layer: {}", pCallbackData->pMessage));

			return VK_FALSE;
		}

		bool checkValidationLayerSupport();
		VkResult createDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void destroyDebugUtilsmessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		
		std::vector<const char*> getRequiredExtensions();
		
		VkPhysicalDevice pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
		int calculatePhysicalDeviceScore(const VkPhysicalDevice& device);
		bool isDeviceSuitable(const VkPhysicalDevice& device);
		bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);
		QueueFamilyIndicies findQueueFamilyIndices(const VkPhysicalDevice& device);
		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		};
	};

#endif