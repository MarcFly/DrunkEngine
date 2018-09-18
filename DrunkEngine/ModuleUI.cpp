#include "Globals.h"
#include "Module.h"
#include "ModuleUI.h"
#include "ModuleInput.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "imgui/implements/imgui_impl_sdl.h"
#include "imgui/implements/imgui_impl_opengl2.h"
#include <gl/GL.h>

#define MEM_BUDGET_NVX 0x9048
#define MEM_AVAILABLE_NVX 0x9049

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
	show_options_window = false;

	window_update = false;

	fullscreen = false;
	resizable = true;
	borderless = false;
	full_desktop = false;

	brightness = 1.0;
	SDL_GetWindowSize(App->window->window, &width, &height);

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
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Options"))
				show_options_window = !show_options_window;

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
	}
	ImGui::EndMainMenuBar();

	CheckOpenWindows();

	return ret;
}

bool ModuleUI::CheckOpenWindows()
{
	bool ret = true;

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	if (show_options_window)
		ShowOptionsWindow();

	if (show_about_window)
		ShowAboutWindow();

	return ret;
}

void ModuleUI::ShowOptionsWindow()
{
	ImGui::Begin("Options", &show_options_window);
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			
		}
		if (ImGui::CollapsingHeader("Windows"))
		{
			ImGui::Checkbox("Active", &window_update);
			if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f) && window_update)
				SDL_SetWindowBrightness(App->window->window, brightness);
			
			if (ImGui::SliderInt("Width", &width, 720, 1920) && window_update && !fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);

			if (ImGui::SliderInt("Height", &height, 480, 1080) && window_update && !fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);
			

			ImGui::Text("Refresh rate: %d", 0);

			if (ImGui::Checkbox("Fullscreen", &fullscreen) && window_update)
			{
				SDL_SetWindowFullscreen(App->window->window, fullscreen);
				//SDL_GetRendererOutputSize(SDL_GetRenderer(App->window->window), &width, &height);
				SDL_SetWindowSize(App->window->window, width, height);
				
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable) && window_update)
				SDL_SetWindowResizable(App->window->window, (SDL_bool)resizable);

			if (ImGui::Checkbox("Borderless", &borderless) && window_update)
				SDL_SetWindowBordered(App->window->window, (SDL_bool)!borderless);

			ImGui::SameLine();
			ImGui::Checkbox("Full Desktop", &full_desktop);
			
		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImGui::Text("SDL Version: ");
			SDL_version ver;
			SDL_GetVersion(&ver);
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d.%d.%d", ver.major, ver.minor, ver.patch);

			ImGui::Separator();

			ImGui::Text("Logical CPUs: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d Threads (%d KB)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
		
			ImGui::Text("System Ram: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f GB", SDL_GetSystemRAM() / 1024.0f);

			ImGui::Text("Caps: ");
			ImGui::SameLine();
			if(SDL_Has3DNow())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "3DNow,");
			ImGui::SameLine();
			if (SDL_HasAVX())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX,");
			ImGui::SameLine();
			if (SDL_HasAVX2())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX2,");
			ImGui::SameLine();
			if (SDL_HasAltiVec())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AltiVec,");
			ImGui::SameLine();
			if (SDL_HasMMX())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "MMX,");
			ImGui::SameLine();
			ImGui::Text("\n");
			if (SDL_HasRDTSC())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "RDTSC,");
			ImGui::SameLine();
			if (SDL_HasSSE())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE,");
			ImGui::SameLine();
			if (SDL_HasSSE2())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE2,");
			ImGui::SameLine();
			if (SDL_HasSSE3())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE3,");
			ImGui::SameLine();
			if (SDL_HasSSE41())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE41,");
			ImGui::SameLine();
			if (SDL_HasSSE42())
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE42");

			ImGui::Separator();

			// GPU
			ImGui::Text("Brand: ");
			ImGui::SameLine(); 
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VENDOR));

			ImGui::Text("GPU: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_RENDERER));

			ImGui::Text("Drivers: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s",glGetString(GL_VERSION));

			ImGui::Text("VRAM Budget: ");
			ImGui::SameLine();
			GLint budget = 0;
			glGetIntegerv(MEM_BUDGET_NVX, &budget);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", budget / 1024.0f);

			ImGui::Text("VRAM Budget: ");
			ImGui::SameLine();
			GLint available = 0;
			glGetIntegerv(MEM_AVAILABLE_NVX, &available);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", available / 1024.0f);
			
		}
	}
	ImGui::End();
}

void ModuleUI::ShowAboutWindow()
{
	ImGui::Begin("About this engine", &show_about_window);
	{
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
	}
	ImGui::End();
}
