#include "Globals.h"
#include "Module.h"
#include "ModuleUI.h"
#include "ModuleInput.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "imgui/implements/imgui_impl_sdl.h"
#include "imgui/implements/imgui_impl_opengl2.h"


ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ImGui::CreateContext();
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init()
{
	LOG("Initiating IMGUI Module")
	bool ret = true;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();
	ImGui::StyleColorsDark();

	return ret;
}

update_status ModuleUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();
	ExitWindow();
	MenuWindow();

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
	LOG("Cleaning UP IMGUI Module")
	bool ret = true;
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return ret;
}

bool ModuleUI::ExitWindow()
{
	ImGui::Begin("Exit App");
	
	if (ImGui::Button("Exit", ImVec2(50, 25)))
	{
		App->input->StopRunning();
	}

	ImGui::End();

	return true;
}

bool ModuleUI::MenuWindow()
{
	ImGui::Begin("Menu");
	ImGui::BeginMenuBar();
	
	if (ImGui::BeginMenu("Examples"))
	{
		if (ImGui::MenuItem("Console"))
			TestMenus();
		if (ImGui::MenuItem("Log"))
			TestMenus();
		if (ImGui::MenuItem("Simple layout"))
			TestMenus();
		if (ImGui::MenuItem("Property editor"))
			TestMenus();
		if (ImGui::MenuItem("Long text display"))
			TestMenus();
		if (ImGui::MenuItem("Auto-resizing window"))
			TestMenus();
		if (ImGui::MenuItem("Constrained-resizing window"))
			TestMenus();
		if (ImGui::MenuItem("Simple overlay"))
			TestMenus();
		if (ImGui::MenuItem("Manipulating window titles"))
			TestMenus();
		if (ImGui::MenuItem("Custom rendering"))
			TestMenus();

		ImGui::EndMenu();
	}
	ImGui::End();

	return true;
}

bool ModuleUI::TestMenus()
{
	ImGui::Begin("dummy menu");
	ImGui::End();
	return true;
}
