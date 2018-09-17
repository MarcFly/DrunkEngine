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

	show_demo_window = false;
	show_about_window = false;

	return ret;
}

update_status ModuleUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	MainMenu();

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

bool ModuleUI::MainMenu()
{
	bool ret = true;

	ImGui::BeginMainMenuBar();
	
	if (ImGui::BeginMenu("Menu"))
	{
		if (ImGui::MenuItem("About..."))
			show_about_window = !show_about_window;
		if (ImGui::MenuItem("Exit"))
			App->input->StopRunning();

		ImGui::EndMenu();
	}
	
	if (ImGui::BeginMenu("Examples"))
	{
		show_demo_window = !show_demo_window;
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	if (show_about_window)
		ShowAboutWindow();

	return ret;
}

void ModuleUI::ShowAboutWindow()
{
	ImGui::Begin("About this engine", &show_about_window);

	ImGui::Text("DrunkEngine (2018).");
	ImGui::Separator();
	ImGui::TextWrapped("\nThis engine has been made as a project for a 3rd course subject on videogame development.\n\n");
	ImGui::Text("Authors: Andreu Sacasas\n         Marc Torres\n\n");
	ImGui::Separator();
	ImGui::Text("\nLibraries used: OpenGL (V 2.1)\n				SDL    (V 2.0.8)\n				ImGui  (V 1.66)\n				Bullet (V 2.84)\n\n");
	ImGui::Separator();
	ImGui::TextWrapped("\nMIT License\n\nCopyright(c) 2018 Marc Torres Jimenez & Andreu Sacasas Velazquez\n\n"
		"Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the 'Software'), to deal "
		"in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
		"copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :\n\n"
		"The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. "
		"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
		"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.'");
	ImGui::End();
}
