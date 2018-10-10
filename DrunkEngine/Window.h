#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Globals.h"
#include "Application.h"
#include "SDL\include\SDL_scancode.h"
#include <string>

class Window
{
public:
	Window(const char* name);
	virtual ~Window();

	void SwitchActive();
	bool IsActive() const;
	std::string GetName() const;

	SDL_Scancode GetShortCut() const;
	void SetShortCut(SDL_Scancode shortcut) { this->shortcut = shortcut; };

	virtual void Draw() = 0;

	void SetInactive() { active = false; };

	virtual bool CleanUp() { return true; };

public:
	bool active = false;
	Application* App;

protected:
	std::string name;
	SDL_Scancode shortcut = SDL_SCANCODE_UNKNOWN;
};

#endif