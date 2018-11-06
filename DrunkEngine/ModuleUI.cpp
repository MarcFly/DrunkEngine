#include "Globals.h"
#include "Module.h"
#include "ModuleUI.h"
#include "ModuleInput.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "imgui/implements/imgui_impl_sdl.h"
#include "imgui/implements/imgui_impl_opengl2.h"
#include <gl/GL.h>
#include <string.h>
#include "Window.h"
#include "OptionsWindow.h"
#include "AboutWindow.h"
#include "RandomGenWindow.h"
#include "GeometryCreationWindow.h"
#include "ConsoleWindow.h"
#include "Inspector.h"
#include "KdTreeWindow.h"
#include "ObjectPropertiesWindow.h"
#include "LoadSaveWindows.h"

#define MEM_BUDGET_NVX 0x9048
#define MEM_AVAILABLE_NVX 0x9049

using namespace std;

ModuleUI::ModuleUI(bool start_enabled) : Module(start_enabled)
{
	ImGui::CreateContext();
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init()
{
	PLOG("Initiating IMGUI Module");
	bool ret = true;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();
	ImGui::StyleColorsDark();

	show_demo_window = false;

	windows.push_back(console_win = new ConsoleWindow());
	windows.push_back(options_win = new OptionsWindow());
	windows.push_back(about_win = new AboutWindow());
	windows.push_back(random_win = new RandomGenWindow());
	windows.push_back(geometry_win = new GEOMWindow());
	windows.push_back(inspector = new Inspector());
	windows.push_back(octree_win = new KDTreeWindow());
	windows.push_back(obj_properties_win = new ObjectPropertiesWindow());
	windows.push_back(savescene_win = new SaveSceneWindow());
	windows.push_back(loadscene_win = new LoadSceneWindow());
	windows.push_back(import_win = new ImportWindow());

	App->input->UpdateShortcuts();

	for (int i = 0; i < NUM_WINDOWS; i++)
		active_windows[i] = false;

	return ret;
}

update_status ModuleUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	MainMenu();

	for (vector<Window*>::iterator it = windows.begin(); it != windows.end(); ++it)
	{
		Window* windows = (*it);

		if (!CheckDataWindows() && App->input->GetKey(windows->GetShortCut()) == KEY_DOWN)
			windows->SwitchActive();

		if (windows->IsActive())
			windows->Draw();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	App->ui->console_win->AddLog("Cleaning UP IMGUI Module");

	for (int i = 0; i < windows.size(); i++)
	{
		windows[i]->CleanUp();
		delete windows[i];
	}
	windows.clear();


	bool ret = true;
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return ret;
}

bool ModuleUI::MainMenu()
{
	bool ret = true;

	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Options"))
				options_win->SwitchActive();				

			if (ImGui::MenuItem("Save Scene"))
			{
				savescene_win->SwitchActive();
			}

			if (ImGui::MenuItem("Load Scene"))
			{
				loadscene_win->SwitchActive();
			}

			if (ImGui::MenuItem("Import..."))
			{
				import_win->SwitchActive();
			}

			if (ImGui::MenuItem("Show/Hide Windows"))
				ShowHideWindows();

			if (ImGui::MenuItem("Exit"))
				App->input->StopRunning();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{

			/*if (ImGui::MenuItem("Random Generator"))
				random_win->SwitchActive();

			if (ImGui::MenuItem("Test Objects"))
				geometry_win->SwitchActive();*/

			if (ImGui::MenuItem("Inspector"))
				inspector->SwitchActive();

			if (ImGui::MenuItem("Object Properties"))
				obj_properties_win->SwitchActive();

			if (ImGui::MenuItem("K-D Tree"))
				octree_win->SwitchActive();			

			if (ImGui::MenuItem("Log"))	//Change to "Console" for Assignment 2
				console_win->SwitchActive();		

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Examples"))
				show_demo_window = !show_demo_window;

			if (ImGui::MenuItem("About..."))
				about_win->SwitchActive();

			if (ImGui::MenuItem("Go to Github"))
				ShellExecute(NULL, "open", "https://github.com/MarcFly/DrunkEngine", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Download Latest"))
				ShellExecute(NULL, "open", "https://github.com/MarcFly/DrunkEngine/releases", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Documentation"))
				ShellExecute(NULL, "open", "https://github.com/MarcFly/DrunkEngine", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Report a Bug"))
				ShellExecute(NULL, "open", "https://github.com/MarcFly/DrunkEngine/issues", NULL, NULL, SW_SHOWNORMAL);

			ImGui::EndMenu();
		}

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		
	}
	ImGui::EndMainMenuBar();


	return ret;
}

void ModuleUI::ShowHideWindows()
{
	bool show_windows = true;

	for (int i = 0; i < windows.size(); i++)
	{
		if (windows[i]->active == true)
		{
			for (int j = 0; j < windows.size(); j++)
				active_windows[j] = false;

			show_windows = false;
		}
	}

	for (int i = 0; i < windows.size(); i++)
	{
		if (!show_windows && windows[i]->active == true)
		{
			active_windows[i] = true;
			windows[i]->SetInactive();
		}

		else if (show_windows && active_windows[i] == true)
		{
			windows[i]->SwitchActive();
			active_windows[i] = false;
		}

	}

	show_demo_window = false;
}

bool ModuleUI::CheckDataWindows()
{
	bool ret = false;

	ret = savescene_win->IsActive();
	if (ret)
		return ret;
	ret = loadscene_win->IsActive();
	if (ret)
		return ret;
	ret = import_win->IsActive();
	if (ret)
		return ret;

	return ret;
}