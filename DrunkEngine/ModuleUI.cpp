#include "Globals.h"
#include "ModuleUI.h"
#include "Application.h"
#include "imgui/imgui.h"
using namespace ImGui;

ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init()
{
	LOG("Initiating IMGUI Module")
	bool ret = true;

	ShowDemoWindow();

	return ret;
}

update_status ModuleUI::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	LOG("Cleaning UP IMGUI Module")
	bool ret = true;

	return ret;
}
