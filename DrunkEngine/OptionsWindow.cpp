#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "OptionsWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "Imgui/imgui.h"
#include <gl/GL.h>

#define MEM_BUDGET_NVX 0x9048
#define MEM_AVAILABLE_NVX 0x9049

OptionsWindow::OptionsWindow(Application* app) : Window("Options", SDL_SCANCODE_UNKNOWN)
{
	App = app;
	key_repeated = false;
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
			int width;
			int height;

			SDL_GetWindowSize(App->window->window, &width, &height);

			if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0.0f, 1.0f))
				App->window->SetBrightness(App->window->brightness);

			if (ImGui::SliderInt("Width", &width, 720, 1920) && !App->window->fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);

			if (ImGui::SliderInt("Height", &height, 480, 1080) && !App->window->fullscreen)
				SDL_SetWindowSize(App->window->window, width, height);


			ImGui::Text("Refresh rate: %d", 0);

			if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen))
			{
				//SDL_SetWindowSize(App->window->window, width, height);
				App->window->SetFullscreen(App->window->fullscreen);
				//SDL_GetRendererOutputSize(SDL_GetRenderer(App->window->window), &width, &height);

			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &App->window->resizable))
				App->window->SetResizable(App->window->resizable);

			if (ImGui::Checkbox("Borderless", &App->window->borderless))
				App->window->SetBorderless(App->window->borderless);

			ImGui::SameLine();
			if (ImGui::Checkbox("Full Desktop", &App->window->full_desktop))
				App->window->SetFullDesktop(App->window->full_desktop);

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
		if (ImGui::CollapsingHeader("Input"))
		{
			ImGui::Text("Camera Controls:");
			
			//Key Repeated Warning
			if (key_repeated == true)
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Key already in use!");
				if (repeated_key_time.Read() > 3000)
					key_repeated = false;
			}

			ImGui::Separator();

			std::string aux;

			//Move Forward
			ImGui::Text("Move Forward");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[MOVE_FORWARD]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = MOVE_FORWARD;

			if (input_change == MOVE_FORWARD)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");	
				key_repeated = false;
			}

			//Move Back
			ImGui::Text("Move Back");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[MOVE_BACK]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = MOVE_BACK;

			if (input_change == MOVE_BACK)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");
				key_repeated = false;
			}

			//Move Left
			ImGui::Text("Move Left");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[MOVE_LEFT]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = MOVE_LEFT;

			if (input_change == MOVE_LEFT)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");
				key_repeated = false;
			}

			//Move Right
			ImGui::Text("Move Right");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[MOVE_RIGHT]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = MOVE_RIGHT;

			if (input_change == MOVE_RIGHT)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");
				key_repeated = false;
			}

			//Focus Camera
			ImGui::Text("Focus Camera");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[FOCUS_CAMERA]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = FOCUS_CAMERA;

			if (input_change == FOCUS_CAMERA)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");
				key_repeated = false;
			}

			//Orbit Camera
			ImGui::Text("Orbit Camera");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[ORBIT_CAMERA]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = ORBIT_CAMERA;

			if (input_change == ORBIT_CAMERA)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");
				key_repeated = false;
			}

			//MENU SHORTCUTS
			ImGui::Separator();
			ImGui::Text("Menu Shortcuts:");
			ImGui::Separator();

			//Options Window
			ImGui::Text("Options Menu");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[OPTIONS_MENU]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = OPTIONS_MENU;

			if (input_change == OPTIONS_MENU)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");
				key_repeated = false;
			}

			//Loaded Models Window
			ImGui::Text("Loaded Models");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[MESH_MENU]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = MESH_MENU;

			if (input_change == MESH_MENU)
			{
				ImGui::SameLine();
				ImGui::Text("Select new Key");
				key_repeated = false;
			}

			//Check New Key
			if (input_change != NULL_CONTROL)
				CheckInputChange();

			ImGui::Separator();

			if (ImGui::Button("Save Changes"))
				App->input->Save(nullptr);
		}
	}
	ImGui::End();
}

void OptionsWindow::CheckInputChange()
{
	ImGuiIO& io = ImGui::GetIO();

	for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
	{
		if (ImGui::IsKeyPressed(i))
		{
			for (int j = 0; j < NULL_CONTROL; j++)
			{
				if (i == App->input->controls[j])
				{
					key_repeated = true;
					repeated_key_time.Start();
					input_change = NULL_CONTROL;
					break;
				}
			}

			if (key_repeated)
				break;

			if (input_change == MOVE_FORWARD)
				App->input->controls[MOVE_FORWARD] = i;

			else if (input_change == MOVE_BACK)
				App->input->controls[MOVE_BACK] = i;

			else if (input_change == MOVE_LEFT)
				App->input->controls[MOVE_LEFT] = i;

			else if (input_change == MOVE_RIGHT)
				App->input->controls[MOVE_RIGHT] = i;

			else if (input_change == FOCUS_CAMERA)
				App->input->controls[FOCUS_CAMERA] = i;

			else if (input_change == ORBIT_CAMERA)
				App->input->controls[ORBIT_CAMERA] = i;

			else if (input_change == OPTIONS_MENU)
				App->input->controls[OPTIONS_MENU] = i;

			else if (input_change == MESH_MENU)
				App->input->controls[MESH_MENU] = i;

			input_change = NULL_CONTROL;
		}
	}
}


