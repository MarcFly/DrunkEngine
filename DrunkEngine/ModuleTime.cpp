#include "ModuleTime.h"
#include "imgui/imgui.h"
#include "Application.h"

ModuleTime::ModuleTime(bool start_enabled) : Module(start_enabled, Type_Time)
{
	game_timer.Start();
	real_timer.Start();
	count_fps = 0;
	fps = 0;

	game_state = TS_Stop;
	game_speed = 1;
	curr_speed = 0;

	UpdateCurrentMode();
}

ModuleTime::~ModuleTime()
{
	// Destructor
}

void ModuleTime::PrepareUpdate()
{
	Frame_Metrics();
	Game_Frame_Metrics();
}

update_status ModuleTime::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleTime::Update(float dt)
{
	if(ImGui::Begin("##TimeWind", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button("PLAY") && game_state < 2)
		{
			game_state = TS_Play;
			UpdateCurrentMode();
			last_saved_scene = App->scene->SaveScene();
		}
		ImGui::SameLine();
		if (ImGui::Button("PAUSE") && game_state != 1)
		{
			game_state = TS_Pause;
			UpdateCurrentMode();
		}
		ImGui::SameLine();
		if (ImGui::Button("STOP") && game_state > 0)
		{
			game_state = TS_Stop;
			UpdateCurrentMode();
			App->scene->LoadSceneFile(last_saved_scene.c_str());
		}
		ImGui::SameLine();
		if (ImGui::Button("Step"))
		{
			game_state = TS_Step;
			UpdateCurrentMode();
		}
		ImGui::SameLine();
		if (ImGui::BeginCombo("##Time2", CurrentMode.c_str()))
		{
			ImGui::PushItemWidth(70);

			if (ImGui::Selectable("Normal", false, 0, ImVec2(50, 20)))
			{
				game_state = TS_Play;
				UpdateCurrentMode();
			}

			if (ImGui::Selectable("Slow", false, 0, ImVec2(50, 20)))
			{
				game_state = TS_Slow;
				UpdateCurrentMode();
			}

			if (ImGui::Selectable("Fast", false, 0, ImVec2(50, 20)))
			{
				game_state = TS_Fast;
				UpdateCurrentMode();
			}

			if (ImGui::Selectable("Reverse", false, 0, ImVec2(50, 20)))
			{
				game_state = TS_Reverse;
				UpdateCurrentMode();
			}

			if (ImGui::Selectable("RSlow", false, 0, ImVec2(50, 20)))
			{
				game_state = TS_Reverse_Slow;
				UpdateCurrentMode();
			}

			if (ImGui::Selectable("RFast", false, 0, ImVec2(50, 20)))
			{
				game_state = TS_Reverse_Fast;
				UpdateCurrentMode();
			}

			ImGui::EndCombo();
		}

		if (game_state > 2 && game_state != TS_Reverse)
			ImGui::DragFloat("Playback Speed", &game_speed, 1, 1, 8);

		ImGui::End();
	}
	


	return UPDATE_CONTINUE;
}

float ModuleTime::GetFPS() const
{
	return 1.0f / real_dt;
}

void ModuleTime::Frame_Metrics()
{
	//dt

	// Very badly constructed delay in order to cap fps
	/*if (dt > 0 && fps_cap > 0 && (dt < 1.0f / (float)fps_cap))
		SDL_Delay(1000*((1.0f / (float)fps_cap) - dt));*/

	real_dt = (float)real_timer.Read() / 1000.0f;
	real_timer.Start();

}

void ModuleTime::Game_Frame_Metrics()
{
	if (game_state != TS_Stop && game_state != TS_Error)
	{
		game_dt = (float)game_timer.Read() / 1000.0f;
		game_timer.Start();
		count_fps++;

		if (game_state == TS_Step)
			game_state = TS_Pause;
	}
}

void ModuleTime::Cap_FPS(const int& cap)
{
	fps_cap = cap;
}

float ModuleTime::DtMil() const
{
	return 1000 * real_dt;
}

float ModuleTime::GetDT() const
{
	return real_dt;
}

float ModuleTime::GameDT() const
{
	switch (game_state)
	{
	case TS_Stop:
	case TS_Pause:
		return 0;
	case TS_Step:
	case TS_Play:	
		return game_dt;
	case TS_Slow:
		return game_dt / game_speed;
	case TS_Fast:
		return game_dt * game_speed;
	case TS_Reverse:
		return game_dt * -1.0f;
	case TS_Reverse_Slow:
		return game_dt / -(game_speed);
	case TS_Reverse_Fast:
		return game_dt * -(game_speed);
	default:
		return 0;
	}
}

float ModuleTime::GameDTMil() const
{
	switch (game_state)
	{
	case TS_Stop:
	case TS_Pause:
		return 0;
	case TS_Step:
	case TS_Play:
		return 1000 * game_dt;
	case TS_Slow:
		return 1000 * game_dt / game_speed;
	case TS_Fast:
		return 1000 * game_dt * game_speed;
	case TS_Reverse:
		return 1000 * game_dt * -1.0f;
	case TS_Reverse_Slow:
		return 1000 * game_dt / -(game_speed);
	case TS_Reverse_Fast:
		return 1000 * game_dt * -(game_speed);
	default:
		return 0;
	}
}

Uint32 ModuleTime::StartupTime() const
{
	return real_timer.Read();
}

void ModuleTime::UpdateCurrentMode()
{
	switch (game_state)
	{
	case TS_Stop:
		CurrentMode = "STOPPED";
		break;
	case TS_Pause:
		CurrentMode = "PAUSED";
		break;
	case TS_Step:
		CurrentMode = "PAUSED";
		break;
	case TS_Play:
		CurrentMode = "PLAYING";
		break;
	case TS_Slow:
		CurrentMode = "PLAYING_SLOW";
		break;
	case TS_Fast:
		CurrentMode = "PLAYING_FAST";
		break;
	case TS_Reverse:
		CurrentMode = "REVERSED";
		break;
	case TS_Reverse_Slow:
		CurrentMode = "REVERSED_SLOW";
		break;
	case TS_Reverse_Fast:
		CurrentMode = "REVERSED_FAST";
		break;
	default:
		CurrentMode = "ERROR";
		break;
	}

}