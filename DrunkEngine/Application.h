#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
//#include "ModulePhysics3D.h"
#include "ModuleScene.h"
#include "ModuleImport.h"

#include "ModuleUI.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleCamera3D* camera;
	ModuleRenderer3D* renderer3D;
	//ModulePhysics3D* physics;
	ModuleImport* importer;
	ModuleUI* ui;
	ModuleScene* scene;

	std::string profile = "config_data.json";

private:

	Timer	ms_timer;
	Timer	fps_timer;
	float	dt;
	int		count_fps;
	int		fps;
	int		fps_cap;
	std::list<Module*> list_modules;

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

extern Application* App;

#endif