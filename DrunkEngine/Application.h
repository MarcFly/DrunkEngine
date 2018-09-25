#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"


#include "ModuleUI.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleUI* ui;

private:

	Timer	ms_timer;
	Timer	fps_timer;
	float	dt;
	int		count_fps;
	int		fps;
	int		fps_cap;
	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetFPS();
	float GetDt();
	void Cap_FPS(const int& cap);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
	void Frame_Metrics();

};

#endif