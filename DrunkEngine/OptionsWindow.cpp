#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "OptionsWindow.h"
#include "ModuleRenderer3D.h"
#include "Imgui/imgui.h"
#include <gl/GL.h>

#define MEM_BUDGET_NVX 0x9048
#define MEM_AVAILABLE_NVX 0x9049

OptionsWindow::OptionsWindow(Application* app) : Window("Options", SDL_SCANCODE_O)
{
	App = app;

	fullscreen = false;
	resizable = true;
	borderless = false;
	full_desktop = false;
	vsync = true;

	brightness = 1.0;
	max_fps = 60;
}

OptionsWindow::~OptionsWindow()
{}

void OptionsWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::Text("Application Name:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), TITLE);
			
			ImGui::Text("Organization:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ORGANIZATION);

			if (ImGui::Checkbox("Vsync", &vsync))
				App->renderer3D->ChangeVsync(vsync);

			if (ImGui::SliderFloat("Max FPS", &max_fps, 0.0f, 120.0f))
			{
				//Modify max_fps
			}

			fps = App->GetFPS();
			char title[15];
			sprintf_s(title, 15, "Framerate %.1f", fps);

			ImGui::PlotHistogram("##framerate", &fps, 1, 0, title, 0.0f, max_fps + 1, ImVec2(310, 100));
		}
		if (ImGui::CollapsingHeader("Windows"))
		{
			SDL_GetWindowSize(App->window->window, &width, &height);

			if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
				SDL_SetWindowBrightness(App->window->window, brightness);

			if (ImGui::SliderInt("Width", &width, 720, 1920) && !fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);

			if (ImGui::SliderInt("Height", &height, 480, 1080) && !fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);


			ImGui::Text("Refresh rate: %d", 0);

			if (ImGui::Checkbox("Fullscreen", &fullscreen))
			{
				SDL_SetWindowFullscreen(App->window->window, fullscreen);
				//SDL_GetRendererOutputSize(SDL_GetRenderer(App->window->window), &width, &height);
				SDL_SetWindowSize(App->window->window, width, height);

			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable))
				SDL_SetWindowResizable(App->window->window, (SDL_bool)resizable);

			if (ImGui::Checkbox("Borderless", &borderless))
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
			if (SDL_Has3DNow())
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
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));

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
