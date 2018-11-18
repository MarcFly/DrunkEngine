#pragma once

#include "Module.h"
#include "Globals.h"
#include "imgui/imgui.h"
#include "imgui/implements/imgui_impl_sdl.h"
#include <vector>

// Module for IMGUI use and implementation
// Includes for imgui when necessary...

#define NUM_WINDOWS 12

class Window;
class OptionsWindow;
class AboutWindow;
class RandomGenWindow;
class GEOMWindow;
class OpenGLOptions;
class ConsoleWindow;
class SceneViewer;
class KDTreeWindow;
class Inspector;
class SaveSceneWindow;
class LoadSceneWindow;
class ImportWindow;

class ModuleUI : public Module
{
public:
	ModuleUI(bool start_enabled = true);

	~ModuleUI();
	
	bool Init();
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void RenderImGui();

	bool MainMenu();

	void ShowHideWindows();

	bool CheckDataWindows();
	//void AddLog();

public:
	//NUM WINDOWS == 12
	OptionsWindow* options_win = nullptr;
	AboutWindow* about_win = nullptr;
	RandomGenWindow* random_win = nullptr;
	GEOMWindow* geometry_win = nullptr;
	OpenGLOptions* opengl_win = nullptr;
	ConsoleWindow* console_win = nullptr;
	SceneViewer* scene_viewer_window = nullptr;
	KDTreeWindow* kdtree_win = nullptr;
	Inspector* inspector_win = nullptr;

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