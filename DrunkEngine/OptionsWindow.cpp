#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "OptionsWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "Imgui/imgui.h"
#include <gl/GL.h>
#include "ModuleScene.h"
#include "ConsoleWindow.h"
#include "ComponentCamera.h"

#include "GLEW/include/GL/wglew.h"

#define MEM_BUDGET_NVX 0x9048
#define MEM_AVAILABLE_NVX 0x9049

OptionsWindow::OptionsWindow() : Window("Options")
{
	key_repeated = false;
	max_fps = 60;

	//ram_read_time.Start();

	App->ui->console_win->AddLog("Created Options Window-----------------");
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

			/*if (ImGui::SliderFloat("Max FPS", &max_fps, 0.0f, 120.0f))
			{
				App->Cap_FPS(max_fps);
			}*/

			char title[20];
			if (frame_read_time.Read() >= 200) {
				
				if (fps_history.size() > fps_h_size)
				{
					for (int i = 1; i < fps_history.size(); i++)
						fps_history[i - 1] = fps_history[i];

					fps_history[fps_history.size() - 1] = App->GetFPS();
				}
				else
					fps_history.push_back(App->GetFPS());

				frame_read_time.Start();
			}
			sprintf_s(title, 20, "Framerate %.2f", fps_history[fps_history.size() - 1]);

			ImGui::PlotHistogram("##framerate", &fps_history[0], fps_h_size, 0, title, 0.0f, (max_fps + 1) * 1.25f, ImVec2(310, 100));

			if (dt_history.size() > dt_h_size)
			{
				for (int i = 1; i < dt_history.size(); i++)
					dt_history[i - 1] = dt_history[i];

				dt_history[dt_history.size() - 1] = App->GetDt();
			}
			else
				dt_history.push_back(App->GetDt());
			
			
			sprintf_s(title, 20, "DT %.2f ms", dt_history[dt_history.size() - 1]);

			ImGui::PlotHistogram("##time_differential", &dt_history[0], dt_h_size, 0, title, 0.0f, (1000*(1.0f / max_fps) + 1) * 1.25f, ImVec2(310, 100));
			
		}
		if (ImGui::CollapsingHeader("Window"))
		{		

			if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0.0f, 1.0f))
				App->window->SetBrightness(App->window->brightness);

			if (ImGui::SliderInt("Width", &App->window->window_w, 400, App->window->screen_size_w - 1) && !App->window->fullscreen)
				SDL_SetWindowSize(App->window->window, App->window->window_w, App->window->window_h);

			if (ImGui::SliderInt("Height", &App->window->window_h, 400, App->window->screen_size_h - 1) && !App->window->fullscreen)
				SDL_SetWindowSize(App->window->window, App->window->window_w, App->window->window_h);


			ImGui::Text("Refresh rate: %d", 0);

			if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen))
			{				
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

			ImGui::Separator();

			if (ImGui::Button("Save Changes"))
				App->window->Save(nullptr);

			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Render Options"))
		{
			ImGui::Text("Tinker with the settings we are currently using...");
			ImGui::Separator();

			if (ImGui::Checkbox("Vsync", &App->renderer3D->vsync))
				App->renderer3D->ChangeVsync();

			if (ImGui::Checkbox("Depth Testing", &App->renderer3D->depth_test))
				CheckEnableDisableOpenGL();
			ImGui::SameLine();

			if (ImGui::Checkbox("Cull Faces", &App->renderer3D->cull_face))
				CheckEnableDisableOpenGL();
			ImGui::SameLine();

			if (ImGui::Checkbox("Lighting", &App->renderer3D->lighting))
				CheckEnableDisableOpenGL();
			ImGui::SameLine();

			if (ImGui::Checkbox("Color Materials", &App->renderer3D->color_material))
				CheckEnableDisableOpenGL();
			ImGui::SameLine();

			if (ImGui::Checkbox("Textures 2D", &App->renderer3D->texture_2d))
				CheckEnableDisableOpenGL();

			ImGui::Checkbox("Render Faces", &App->renderer3D->faces);
			ImGui::SameLine();

			ImGui::Checkbox("Render Wireframe", &App->renderer3D->wireframe);
			ImGui::SameLine();

			ImGui::Checkbox("Render Bounding Box", &App->renderer3D->bounding_box);
			ImGui::SameLine();

			ImGui::Checkbox("Show Normals", &App->renderer3D->render_normals);

			ImGui::SliderFloat("Normal Length", &App->renderer3D->normal_length, 0.1f, 2.0f);

			ImGui::Separator();

			if (ImGui::Button("Save Changes"))
				App->renderer3D->Save(nullptr);

			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Separator();

			float aux_fov = RadToDeg(App->scene->Main_Cam->frustum.verticalFov);
			if (ImGui::SliderFloat("FOV", &aux_fov, 0.1f, 179.0f))
			{
				App->scene->Main_Cam->frustum.verticalFov = DegToRad(aux_fov);
				App->scene->Main_Cam->SetAspectRatio();
				App->renderer3D->OnResize();
			}

			if (ImGui::SliderFloat("NearPlane", &App->scene->Main_Cam->frustum.nearPlaneDistance, 0.5f, 200.0f))
			{
				App->scene->Main_Cam->frustum.SetViewPlaneDistances(App->scene->Main_Cam->frustum.nearPlaneDistance, App->scene->Main_Cam->frustum.farPlaneDistance);
				App->renderer3D->OnResize();
			}

			if (ImGui::SliderFloat("FarPlane", &App->scene->Main_Cam->frustum.farPlaneDistance, 1.f, 1000.0f))
			{
				App->scene->Main_Cam->frustum.SetViewPlaneDistances(App->scene->Main_Cam->frustum.nearPlaneDistance, App->scene->Main_Cam->frustum.farPlaneDistance);
				App->renderer3D->OnResize();
			}		
		}

		if (ImGui::CollapsingHeader("Texture Parameters"))
		{
			ImGui::TextWrapped("In order to perform changes, drop a new texture (or the same one again)!");
			ImGui::Separator();

			const char* wrap_types[] = { "CLAMP TO EDGE", "CLAMP TO BORDER", "MIRRORED REPEAT", "REPEAT", "MIRROR CLAMP" };
			if (ImGui::Combo("Texture Wrap S", &App->renderer3D->curr_tws, wrap_types, IM_ARRAYSIZE(wrap_types)))
				App->renderer3D->SetTextureParams();
			if (ImGui::Combo("Texture Wrap T", &App->renderer3D->curr_twt, wrap_types, IM_ARRAYSIZE(wrap_types)))
				App->renderer3D->SetTextureParams();
			
			ImGui::Separator();
			
			const char* tex_f[] = { "NEAREST","LINEAR"/*,"NEAREST MIPMAP NEAREST", "LINEAR MIPMAP NEAREST", "NEAREST MIPMAP LINEAR", "LINEAR MIPMAP LINEAR"*/ };
			if (ImGui::Combo("Texture Min Filter", &App->renderer3D->curr_tminf, tex_f, IM_ARRAYSIZE(tex_f)))
				App->renderer3D->SetTextureParams();

			if (ImGui::Combo("Texture Mag Filter", &App->renderer3D->curr_tmagf, tex_f, IM_ARRAYSIZE(tex_f)))
				App->renderer3D->SetTextureParams();

			ImGui::Separator();

			if (ImGui::Button("Save Changes"))
				App->scene->Save(nullptr);

			ImGui::Separator();
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

			//Console Window
			ImGui::Text("Log Window"); //TODO Change to console for asg 2
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[CONSOLE_MENU]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = CONSOLE_MENU;

			if (input_change == CONSOLE_MENU)
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

			//About Window
			ImGui::Text("Engine Info");
			aux = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)App->input->controls[ABOUT_MENU]));
			ImGui::SameLine();

			if (ImGui::Button(aux.c_str()))
				input_change = ABOUT_MENU;

			if (input_change == ABOUT_MENU)
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
			ImGui::SameLine();
			if (ImGui::Button("Reset to Default"))
				App->input->SetDefaultControls();

			ImGui::Separator();
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

			ImGui::Text("Used System Ram: ");
			ImGui::SameLine();
			if (ram_read_time.Read() > 1000) {
				GetProcessMemoryInfo(GetCurrentProcess(), &mem, sizeof(mem));
				ram_read_time.Start();
			}
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB",  mem.WorkingSetSize / (1024.0f * 1024.0f));


			ImGui::Text("Total System Ram: ");
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
			std::string gpu_vendor = (char*)glGetString(GL_VENDOR);
			
			ImGui::Text("Brand: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", gpu_vendor.c_str());

			ImGui::Text("GPU: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_RENDERER));

			ImGui::Text("Drivers: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));


			// VRAM DATA
			GLint budget = 0;
			GLint available = 0;
			GLuint used_mem = 0;

			if (gpu_vendor.find("NVIDIA") != -1)
			{
				glGetIntegerv(MEM_BUDGET_NVX, &budget);
				glGetIntegerv(MEM_AVAILABLE_NVX, &available);

				used_mem = (budget - available) / 1024;
			}
			else if (gpu_vendor.find("ATI") != -1)
			{
				// With these ones, the info may not be accurate, no better way besides just counting the vram used manually
				GLint vbo_mem[4];
				GLint tex_mem[4];
				GLint rbo_mem[4];
				glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, vbo_mem);
				glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, tex_mem);
				glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, rbo_mem);

				used_mem = (vbo_mem[0] + tex_mem[0] + rbo_mem[0]) / (1024 * 1024);

				wglGetGPUInfoAMD(wglGetGPUIDsAMD(0, 0), WGL_GPU_RAM_AMD, GL_UNSIGNED_INT, sizeof(GLuint), &budget);

				available = budget - used_mem;
				budget *= 1024;
				available *= 1024;
			}
			
			ImGui::Text("VRAM Used: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d MB", used_mem);

			ImGui::Text("VRAM Available: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", available / 1024.0f);

			ImGui::Text("VRAM Total: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", budget / 1024.0f);

		}

		if (ImGui::CollapsingHeader("Libraries Used"))
		{
			// Get Library Versions
			SDL_version sdl_ver;
			SDL_GetVersion(&sdl_ver);
			int major, minor;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);

			ImGui::Text("LIBRARIES USED:");

			ImGui::Separator();

			ImGui::TextColored(HyperlinkColor, "OpenGL (%d.%d) ", major, minor);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "https://www.opengl.org/", NULL, NULL, SW_SHOWNORMAL);

			ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "SDL (%d.%d.%d) ", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "https://www.libsdl.org/", NULL, NULL, SW_SHOWNORMAL);

			ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "ImGui  (%.2f) ", IMGUI_VERSION_NUM / 10000.0f);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);

			/*ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "Bullet (%.2f) ", btGetVersion() / 100.0f);
			if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://github.com/bulletphysics", NULL, NULL, SW_SHOWNORMAL);
			*/
			ImGui::TextColored(HyperlinkColor, "GLEW(%d.%d.%d ", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "http://glew.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);

			ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "DevIL (%d.%d.%D) ", App->scene->GetDevILVer() / 100, App->scene->GetDevILVer() / 10 - 10 * (App->scene->GetDevILVer() / 100), App->scene->GetDevILVer() - 10 * (App->scene->GetDevILVer() / 10));
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);

			ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "Assimp (%d.%d.%d) ", App->scene->GetAssimpMajorVer(), App->scene->GetAssimpMinorVer(), App->scene->GetAssimpVersionRevision());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "http://www.assimp.org/", NULL, NULL, SW_SHOWNORMAL);

			
			ImGui::TextColored(HyperlinkColor, "MathGeoLib ");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL);

			ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "GLMATH ");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "http://glmath.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);

			ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "PCG_RNG ");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "http://www.pcg-random.org/", NULL, NULL, SW_SHOWNORMAL);

			ImGui::SameLine();
			ImGui::TextColored(HyperlinkColor, "Parson");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Go to webpage");
			if (ImGui::IsItemClicked())
				ShellExecute(NULL, "open", "https://github.com/kgabis/parson", NULL, NULL, SW_SHOWNORMAL);
		}
	}
	ImGui::End();
}

void OptionsWindow::CheckEnableDisableOpenGL()
{
	if (glIsEnabled(GL_DEPTH_TEST) && App->renderer3D->depth_test == false)
		glDisable(GL_DEPTH_TEST);
	else if (!glIsEnabled(GL_DEPTH_TEST) && App->renderer3D->depth_test == true)
		glEnable(GL_DEPTH_TEST);

	if (glIsEnabled(GL_CULL_FACE) && App->renderer3D->cull_face == false)
		glDisable(GL_CULL_FACE);
	else if (!glIsEnabled(GL_CULL_FACE) && App->renderer3D->cull_face == true)
		glEnable(GL_CULL_FACE);

	if (glIsEnabled(GL_LIGHTING) && App->renderer3D->lighting == false)
		glDisable(GL_LIGHTING);
	else if (!glIsEnabled(GL_LIGHTING) && App->renderer3D->lighting == true)
		glEnable(GL_LIGHTING);

	if (glIsEnabled(GL_COLOR_MATERIAL) && App->renderer3D->color_material == false)
		glDisable(GL_COLOR_MATERIAL);
	else if (!glIsEnabled(GL_COLOR_MATERIAL) && App->renderer3D->color_material == true)
		glEnable(GL_COLOR_MATERIAL);

	if (glIsEnabled(GL_TEXTURE_2D) && App->renderer3D->texture_2d == false)
		glDisable(GL_TEXTURE_2D);
	else if (!glIsEnabled(GL_TEXTURE_2D) && App->renderer3D->texture_2d == true)
		glEnable(GL_TEXTURE_2D);
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

			else if (input_change == CONSOLE_MENU)
				App->input->controls[CONSOLE_MENU] = i;

			else if (input_change == MESH_MENU)
				App->input->controls[MESH_MENU] = i;

			else if (input_change == ABOUT_MENU)
				App->input->controls[ABOUT_MENU] = i;

			App->input->UpdateShortcuts();

			input_change = NULL_CONTROL;
		}
	}
}


