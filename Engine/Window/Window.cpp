#include "Window.h"

#include <iostream>
#include <format>
#include <thread>

#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#include "../Initializer/Initializer.h"

/**
* Constructor
*/
Window::Window()
{
	_window = nullptr;
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
		SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI
	);

	_stop = true;

	if (_window == nullptr)
	{
		throw std::runtime_error(std::format("Failed to initialize window: {}", SDL_GetError()));
	}

	engine::vulkan::createInstance(_window);
	engine::vulkan::setupDebugMessenger();
	engine::vulkan::getExtensions();
	engine::vulkan::selectPhysicalDevice();
	engine::vulkan::createLogicalDevice();
}

/**
* Run window gameloop
*/
void Window::run()
{
	_stop = false;

	SDL_Event event;
	while (!_stop)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				_stop = true;
				break;

			default:
				break;
			}
		}
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
