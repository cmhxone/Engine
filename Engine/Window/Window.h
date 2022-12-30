#ifndef _ENGINE_WINDOW_HEADER_
#define _ENGINE_WINDOW_HEADER_

#include <string_view>
#include <vector>

#include <SDL2/SDL.h>

using namespace std::string_view_literals;

class Window
{
public:
	Window();
	~Window();

	void init();

	void setWidth(const int width);
	void setHeight(const int height);
	void setTitle(const std::string_view title);

	constexpr int getWidth() const { return _width; }
	constexpr int getHeight() const { return _height; }
	constexpr std::string_view getTitle() const { return _title; }
	constexpr SDL_Window* getWindow() const { return _window; }

protected:

private:
	SDL_Window* _window;

	int _width = 640;
	int _height = 480;

	std::string_view _title = "window"sv;

	void initVulkan();
};

#endif // !_ENGINE_WINDOW_HEADER_
