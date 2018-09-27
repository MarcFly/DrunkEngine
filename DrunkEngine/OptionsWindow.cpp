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

	fullscreen = App->window->GetFullscreen();
	resizable = App->window->GetResizable();
	borderless = App->window->GetBorderless();
	full_desktop = App->window->GetFullDesktop();

	brightness = App->window->GetBrightness();
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

			ImGui::Checkbox("Vsync", &App->renderer3D->vsync);
			App->renderer3D->ChangeVsync();

			if (ImGui::SliderFloat("Max FPS", &max_fps, 0.0f, 120.0f))
			{
				App->Cap_FPS(max_fps);
			}

			char title[20];
			if (frame_read_time.Read() >= 200) {
				if(fps_history.size() < 25)
					fps_history.push_back(App->GetFPS());
				else
				{
					for (int i = 1; i < fps_history.size(); i++)
						fps_history[i - 1] = fps_history[i];

					--*fps_history.end()._Ptr = App->GetFPS();
				}
				frame_read_time.Start();
			}
			sprintf_s(title, 20, "Framerate %.2f", fps_history[fps_history.size() - 1]);

			
			ImGui::PlotHistogram("##framerate", &fps_history[0], fps_history.size(), 0, title, 0.0f, max_fps + 1, ImVec2(310, 100));
		
			if (dt_history.size() > 60) 
			{
				for (int i = 1; i < dt_history.size(); i++)
					dt_history[i-1] = dt_history[i];

				--*dt_history.end()._Ptr = App->GetDt();
			}
			else
				dt_history.push_back(App->GetDt());
			
			sprintf_s(title, 20, "DT %.2f ms", dt_history[dt_history.size() - 1]);

			ImGui::PlotHistogram("##time_differential", &dt_history[0], dt_history.size(), 0, title, 0.0f, 1000*(1.0f / max_fps) + 1, ImVec2(310, 100));
			
		}
		if (ImGui::CollapsingHeader("Windows"))
		{
			SDL_GetWindowSize(App->window->window, &width, &height);

			if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
				App->window->SetBrightness(brightness);

			if (ImGui::SliderInt("Width", &width, 720, 1920) && !fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);

			if (ImGui::SliderInt("Height", &height, 480, 1080) && !fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);


			ImGui::Text("Refresh rate: %d", 0);

			if (ImGui::Checkbox("Fullscreen", &fullscreen))
			{
				App->window->SetFullscreen(fullscreen);
				//SDL_GetRendererOutputSize(SDL_GetRenderer(App->window->window), &width, &height);
				//SDL_SetWindowSize(App->window->window, width, height);

			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable))
				App->window->SetResizable(resizable);

			if (ImGui::Checkbox("Borderless", &borderless))
				App->window->SetBorderless(borderless);

			ImGui::SameLine();
			if (ImGui::Checkbox("Full Desktop", &full_desktop))
				App->window->SetFullDesktop(full_desktop);

			if (ImGui::Button("Save Changes"))
				App->window->Save(nullptr);
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
