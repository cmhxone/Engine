#include <iostream>
#include <vector>
#include <exception>
#include <format>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "Window/Window.h"
#include "Initializer/Initializer.h"
#include "IniReader/IniReader.h"

int main(int argc, char* argv[])
{
	VkInstance instance = nullptr;

	IniReader* reader = new IniReader();
	reader->getInstance();

	try
	{
		engine::initialize(instance);
	}
	catch (std::exception e)
	{
		spdlog::error(std::format("{}", e.what()));
		return EXIT_FAILURE;
	}

	Window* window = new Window();
	window->setWidth(1024);
	window->setHeight(768);
	window->setTitle("Hello world");
	try
	{
		window->init();
	}
	catch (std::exception e)
	{
		spdlog::error(std::format("{}", e.what()));
		return EXIT_FAILURE;
	}

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

	engine::destroy(instance);

	return EXIT_SUCCESS;
}