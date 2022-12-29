#include <iostream>
#include <vector>
#include <exception>
#include <format>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "Window/Window.h"

void initVulkan();
void destroy();

VkInstance instance;

int main(int argc, char* argv[])
{

	try
	{
		initVulkan();
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	Window* window = new Window();
	window->setWidth(1024);
	window->setHeight(768);
	window->setTitle("Hello world");
	window->init();

	bool stop = false;
	SDL_Event event;
	while (!stop)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				stop = true;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDL_KeyCode::SDLK_RIGHT:
					window->setWidth(window->getWidth() + 8);
					break;

				case SDL_KeyCode::SDLK_LEFT:
					window->setWidth(window->getWidth() - 8);
					break;

				case SDL_KeyCode::SDLK_UP:
					window->setHeight(window->getHeight() - 8);
					break;

				case SDL_KeyCode::SDLK_DOWN:
					window->setHeight(window->getHeight() + 8);
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
	}

	try
	{
		destroy();
	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void initVulkan()
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
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

	const VkInstanceCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appinfo,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = 0,
		.ppEnabledExtensionNames = nullptr
	};

	VkResult res;
	res = vkCreateInstance(&info, nullptr, &instance);

	if (res != VkResult::VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vkInstance");
	}
}

void destroy()
{
	vkDestroyInstance(instance, nullptr);
	SDL_Quit();
}