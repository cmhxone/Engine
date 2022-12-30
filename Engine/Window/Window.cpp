#include "Window.h"

#include <iostream>
#include <format>

#include <SDL2/SDL_vulkan.h>

#include "spdlog/spdlog.h"

/**
* Constructor
*/
Window::Window()
{
	_window = nullptr;
	_width = 800;
	_height = 600;
	_title = "Engine"sv;
}

/**
* Destructor
*/
Window::~Window()
{
	SDL_DestroyWindow(_window);
}

/**
* Initialize and create window
*/
void Window::init()
{
	_window = SDL_CreateWindow(
		_title.data(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_width,
		_height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI
	);

	if (_window == nullptr)
	{
		throw std::runtime_error(std::format("Failed to initialize window: {}", SDL_GetError()));
	}

	try
	{
		initVulkan();
	}
	catch (std::exception e)
	{
		throw e;
	}
}

/**
* Resize window screen width
* @param width
*/
void Window::setWidth(const int width)
{
	_width = width <= 0 ? 1 : width;

	if (_window != nullptr)
	{
		int width;
		int height;

		SDL_Vulkan_GetDrawableSize(_window, &width, &height);
		SDL_SetWindowSize(_window, _width, height);
	}
}

/**
* Resize window screen height
* @param height
*/
void Window::setHeight(const int height)
{
	_height = height <= 0 ? 1 : height;

	if (_window != nullptr)
	{
		int width;
		int height;

		SDL_Vulkan_GetDrawableSize(_window, &width, &height);
		SDL_SetWindowSize(_window, width, _height);
	}
}

/**
* Rename window screen title
*/
void Window::setTitle(const std::string_view title)
{
	_title = title;

	if (_window != nullptr)
	{
		SDL_SetWindowTitle(_window, title.data());
	}
}

/**
* initialize vulkan for SDL window
*/
void Window::initVulkan()
{
	// Get Vulkan extensions
	uint32_t extensionCount;
	std::vector<const char*> extensionNames;

	if (SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, nullptr) != SDL_TRUE)
	{
		throw std::runtime_error(std::format("Failed to get vulkan extensions: {}", SDL_GetError()));
	}

	if (SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, extensionNames.data()) != SDL_TRUE)
	{
		throw std::runtime_error(std::format("Failed to get names of vulkan extensions: {}", SDL_GetError()));
	}

	for (const char* extension : extensionNames)
	{
		spdlog::debug(std::format("vulkan extension {} loaded", extension));
	}
}
