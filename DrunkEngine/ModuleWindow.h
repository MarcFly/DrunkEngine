#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

	void SetTitle(const char* title);

	void SetFullscreen(bool fullscreen);
	void SetResizable(bool resizable);
	void SetBorderless(bool borderless);
	void SetFullDesktop(bool full_desktop);
	void SetBrightness(float brightness);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	int window_w;
	int window_h;

	int screen_size_w;
	int screen_size_h;

	bool fullscreen;
	bool resizable;
	bool borderless;
	bool full_desktop;
	float brightness;

private:
	void SetDefault()
	{
		window_h = 100;
		window_w = 100;

		brightness = 1;
		fullscreen = false;
		resizable = true;
		borderless = false;
		full_desktop = false;
	}
};

#endif // __ModuleWindow_H__