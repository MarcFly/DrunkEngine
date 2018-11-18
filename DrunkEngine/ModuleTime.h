#ifndef _Module_Time_
#define _Module_Time_

#include "Module.h"
#include "Timer.h"

enum TimeState
{
	TS_Error = -1,
	TS_Stop,
	TS_Pause,
	TS_Step,
	TS_Play,
	TS_Slow,
	TS_Fast,
	TS_Reverse,
	TS_Reverse_Slow,
	TS_Reverse_Fast,

	TS_Max
};

class ModuleTime : public Module
{
public:
	ModuleTime(bool start_enabled = true);
	~ModuleTime();

	void PrepareUpdate();
	update_status PreUpdate(float dt);
	update_status Update(float dt);

	// Real Timer
	float GetFPS() const;
	float DtMil() const; // Returns DT in miliseconds
	float GetDT() const;	// Return DT in seconds
	void Cap_FPS(const int& cap);

	Uint32 StartupTime() const;

	// Game Timer
	float GameDT() const;
	float GameDTMil() const;

private:
	// Real Timer
	Timer	real_timer;
	Timer	game_timer;
	float	real_dt;
	int		count_fps;
	int		fps;
	int		fps_cap;

	// Game Timer
	float game_speed;
	float game_dt;
	TimeState game_state;
	

	// Timer Controls
	int curr_speed;
	std::string CurrentMode;
	std::string last_saved_scene;

private:
	void Frame_Metrics();
	void Game_Frame_Metrics();
	void UpdateCurrentMode();
};

#endif