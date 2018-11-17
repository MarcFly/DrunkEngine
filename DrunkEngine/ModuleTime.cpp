#include "ModuleTime.h"
#include "imgui/imgui.h"

ModuleTime::ModuleTime(bool start_enabled) : Module(start_enabled, Type_Time)
{
	game_timer.Start();
	real_timer.Start();
	count_fps = 0;
	fps = 0;

	game_state = TS_Play;
	game_speed = 1;
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

update_status ModuleTime::PreUpdate()
{

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
		return 0;
	case TS_Play:
		return game_dt;
	case TS_Pause:
		return 0;
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
		return 0;
	case TS_Play:
		return 1000 * game_dt;
	case TS_Pause:
		return 0;
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
