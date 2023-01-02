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
	struct QueueFamilyIndicies
	{
		std::optional<uint32_t> graphicsFamily;

		constexpr const bool isComplete() const
		{
			return graphicsFamily.has_value();
		}
	};

	class Engine : public Singleton<Engine>
	{
	public:
		Engine();
		~Engine();

		void createInstance(SDL_Window* window);
		void setupDebugMessenger();
		void getExtensions();
		void selectPhysicalDevice();
		void createLogicalDevice();
		void destroyInstance();

	protected:

	private:
		VkInstance _instance = nullptr;
		VkDebugUtilsMessengerEXT _debugMessaenger = nullptr;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkQueue _graphicsQueue = VK_NULL_HANDLE;

		const std::vector<const char*> _validationLayers = {
			"VK_LAYER_KHRONOS_validation"
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
		std::vector<const char*> getRequiredExtensions(SDL_Window* window);
		VkPhysicalDevice pickSuitablePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
		int calculatePhysicalDeviceScore(const VkPhysicalDevice& device);
		bool isDeviceSuitable(const VkPhysicalDevice& device);
		QueueFamilyIndicies findQueueFamilyIndices(const VkPhysicalDevice& device);

	};
};

#endif