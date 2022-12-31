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
	IniReader* reader = new IniReader();
	reader->getInstance();

	try
	{
		engine::initialize();
	}
	catch (const std::exception& e)
	{
		spdlog::error(std::format("{}", e.what()));
		return EXIT_FAILURE;
	}

	Window* window = new Window();
	window->setTitle(reader->getInstance()->getReader().GetString("window", "title", "window"));
	window->setWidth(reader->getInstance()->getReader().GetInteger("window", "width", 640));
	window->setHeight(reader->getInstance()->getReader().GetInteger("window", "height", 480));

	try
	{
		window->init();
	}
	catch (const std::exception& e)
	{
		spdlog::error(std::format("{}", e.what()));
		return EXIT_FAILURE;
	}

	window->run();

	engine::destroy();

	return EXIT_SUCCESS;
}