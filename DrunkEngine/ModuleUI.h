#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "imgui/implements/imgui_impl_sdl.h"
#include <vector>

// Module for IMGUI use and implementation
// Includes for imgui when necessary...

#define NUM_WINDOWS 13

class Window;
class OptionsWindow;
class AboutWindow;
class RandomGenWindow;
class GEOMWindow;
class OpenGLOptions;
class ConsoleWindow;
class Inspector;
class KDTreeWindow;
class ObjectPropertiesWindow;
class SaveSceneWindow;
class LoadSceneWindow;
class ImportWindow;

class ModuleUI : public Module
{
public:
	ModuleUI(bool start_enabled = true);

	~ModuleUI();
	
	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool MainMenu();

	void ShowHideWindows();

	bool CheckDataWindows();
	//void AddLog();

public:
	//NUM WINDOWS == 10
	OptionsWindow* options_win = nullptr;
	AboutWindow* about_win = nullptr;
	RandomGenWindow* random_win = nullptr;
	GEOMWindow* geometry_win = nullptr;
	OpenGLOptions* opengl_win = nullptr;
	ConsoleWindow* console_win = nullptr;
	Inspector* inspector = nullptr;
	KDTreeWindow* octree_win = nullptr;
	ObjectPropertiesWindow* obj_properties_win = nullptr;

	// Data Windows
	SaveSceneWindow* savescene_win = nullptr;
	LoadSceneWindow* loadscene_win = nullptr;
	ImportWindow * import_win = nullptr;

private:
	bool show_demo_window;
	std::vector<Window *> windows;

	bool active_windows[NUM_WINDOWS]; //UPDATE WITH NUM WINDOWS


public:

	const ImVec4 clear_color = { 0.45f, 0.55f, 0.60f, 1.00f };

public:
	void ProcessEvent(SDL_Event* event) { ImGui_ImplSDL2_ProcessEvent(event); }

};