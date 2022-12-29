#ifndef _ENGINE_WINDOW_HEADER_
#define _ENGINE_WINDOW_HEADER_

#include <string_view>
#include <vector>

#include <SDL2/SDL.h>

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

	int _width;
	int _height;

	std::string_view _title;

	std::vector<const char*> _extensionNames;

	void initVulkan();
};

#endif // !_ENGINE_WINDOW_HEADER_
