#include "ModuleTime.h"

ModuleTime::ModuleTime(bool start_enabled) : Module(start_enabled, Type_Time)
{
	fps_timer.Start();
	ms_timer.Start();
	count_fps = 0;
	fps = 0;
}

ModuleTime::~ModuleTime()
{
	// Destructor
}


float ModuleTime::GetFPS()
{
	return 1.0f / dt;
}

void ModuleTime::Frame_Metrics()
{
	//dt

	// Very badly constructed delay in order to cap fps
	/*if (dt > 0 && fps_cap > 0 && (dt < 1.0f / (float)fps_cap))
		SDL_Delay(1000*((1.0f / (float)fps_cap) - dt));*/

	dt = (float)ms_timer.Read() / 1000.0f;

	ms_timer.Start();

}

void ModuleTime::Cap_FPS(const int& cap)
{
	fps_cap = cap;
}

float ModuleTime::GetDt()
{
	return 1000 * dt;
}
