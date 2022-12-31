#ifndef _ENGINE_WINDOW_HEADER_
#define _ENGINE_WINDOW_HEADER_

#include <string>
#include <vector>

#include <SDL2/SDL.h>

class Window
{
public:
	Window();
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void init();
	void run();

	void setWidth(const int width);
	void setHeight(const int height);
	void setTitle(const std::string_view title);

	constexpr int getWidth() const { return _width; }
	constexpr int getHeight() const { return _height; }
	constexpr std::string_view getTitle() const { return _title; }
	constexpr SDL_Window* getWindow() const { return _window; }
	constexpr bool isStop() const { return _stop; }

protected:

private:
	SDL_Window* _window;

	bool _stop = false;

	int _width = 640;
	int _height = 480;

	std::string _title = "window";
};

#endif // !_ENGINE_WINDOW_HEADER_
