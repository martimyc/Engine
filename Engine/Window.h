#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL2/include/SDL.h"

class Application;

class Window : public Module
{
private:
	int screen_width = 0;
	int screen_height = 0;
	int scale = 1;
	bool fullscreen = false;
	bool resizable = false;
	bool borderless = false;
	bool fullscreen_desktop = false;

	bool config_window = true;
	float aspect_ratio;

public:

	Window(const char* name, bool start_enabled = true);

	// Destructor
	virtual ~Window();

	bool Init();
	UPDATE_STATUS Configuration(float dt);
	bool CleanUp();

	void OpenCloseConfigWindowWindow();

	const int GetHeight() const;
	const int GetWidth() const;
	const int GetScale() const;
	const float GetAspectRatio() const;	//width/height

	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;
	SDL_GLContext context;

	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__