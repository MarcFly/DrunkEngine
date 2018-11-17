#ifndef _Module_Time_
#define _Module_Time_

#include "Module.h"
#include "Timer.h"

class ModuleTime : public Module
{
public:
	ModuleTime(bool start_enabled = true);
	~ModuleTime();


	float GetFPS();
	float GetDt();
	void Cap_FPS(const int& cap);

protected:
	Timer	ms_timer;
	Timer	fps_timer;
	float	dt;
	int		count_fps;
	int		fps;
	int		fps_cap;

private:
	void Frame_Metrics();
};

#endif