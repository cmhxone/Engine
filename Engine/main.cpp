#include <iostream>
#include <vector>
#include <exception>
#include <format>

#include <SDL3/SDL_main.h>
#include <spdlog/spdlog.h>

#include "Window/Window.h"
#include "Engine/Engine.h"
#include "IniReader/IniReader.h"

int main(int argc, char* argv[])
{
	try
	{
		engine::Engine::getInstance();
	}
	catch (const std::exception& e)
	{
		spdlog::error(std::format("{}", e.what()));
		return EXIT_FAILURE;
	}

	Window* window = new Window();
	window->setTitle(IniReader::getInstance()->getReader().GetString("window", "title", "window"));
	window->setWidth(IniReader::getInstance()->getReader().GetInteger("window", "width", 640));
	window->setHeight(IniReader::getInstance()->getReader().GetInteger("window", "height", 480));

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

	engine::Engine::destoryInstance();

	return EXIT_SUCCESS;
}