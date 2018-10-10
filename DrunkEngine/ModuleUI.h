#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "imgui/implements/imgui_impl_sdl.h"
#include <vector>

// Module for IMGUI use and implementation
// Includes for imgui when necessary...

#define NUM_WINDOWS 7

class Window;
class OptionsWindow;
class AboutWindow;
class RandomGenWindow;
class GEOMWindow;
class OpenGLOptions;
class ConsoleWindow;
class GeoTransformWindow;

class ModuleUI : public Module
{
public:
	ModuleUI(Application* app, bool start_enabled = true);

	~ModuleUI();
	
	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool MainMenu();

	void ShowHideWindows();
	//void AddLog();

public:
	//NUM WINDOWS == 7
	OptionsWindow* options_win = nullptr;
	AboutWindow* about_win = nullptr;
	RandomGenWindow* random_win = nullptr;
	GEOMWindow* geometry_win = nullptr;
	OpenGLOptions* opengl_win = nullptr;
	ConsoleWindow* console_win = nullptr;
	GeoTransformWindow* geo_transform_win = nullptr;

private:
	bool show_demo_window;
	std::vector<Window *> windows;

	bool active_windows[NUM_WINDOWS]; //UPDATE WITH NUM WINDOWS

public:

	const ImVec4 clear_color = { 0.45f, 0.55f, 0.60f, 1.00f };

public:
	void ProcessEvent(SDL_Event* event)
	{
		ImGui_ImplSDL2_ProcessEvent(event);
	}

};