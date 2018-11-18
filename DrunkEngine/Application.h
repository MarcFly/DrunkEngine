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
#include "ModuleScene.h"
#include "ModuleImport.h"
#include "ModuleEventSystem.h"
#include "ModuleGameObject.h"
#include "ModuleResourceManager.h"
#include "ModuleTime.h"

#include "ModuleUI.h"

class Application
{
public:
	ModuleTime* time;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleCamera3D* camera;
	ModuleRenderer3D* renderer3D;
	ModuleImport* importer;
	ModuleResourceManager* resources;
	ModuleUI* ui;
	ModuleScene* scene;
	ModuleEventSystem* eventSys;
	ModuleGameObject* gameObj;

	std::string profile = "config_data.json";

private:
	std::list<Module*> list_modules;

	Timer	DebugT;

	bool _isEditor;
public:

	Application();
	~Application();

	bool Init();
	bool PreUpdate();
	update_status Update();
	bool DoUpdate();
	bool PostUpdate();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
	
	void EventSystemBroadcast();
};

extern Application* App;

#endif