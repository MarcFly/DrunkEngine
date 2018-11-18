#include "Application.h"
#include "ConsoleWindow.h"
#include "parson/parson.h"

Application::Application()
{
#if GAME
	_isEditor = false;
#else
	_isEditor = true;
#endif

	time = new ModuleTime(this);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	importer = new ModuleImport();
	resources = new ModuleResourceManager(this);
	ui = new ModuleUI(this);
	scene = new ModuleScene(this);
	eventSys = new ModuleEventSystem(this);
	gameObj = new ModuleGameObject(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(time);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(importer);
	AddModule(resources);
	AddModule(scene);
	AddModule(gameObj);
	AddModule(ui);
	

	// Renderer last!
	AddModule(renderer3D);

	AddModule(eventSys);

	App = this;
}

Application::~Application()
{
	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end())
	{
		delete item._Ptr->_Myval;
		item++;
	}
	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// 
	// After all Start we load data if there is a data file
	PLOG("Application Load profile --------------");

	DebugT.Start();

	std::list<Module*>::iterator item = list_modules.begin();

	JSON_Value* root_v = json_parse_file(profile.c_str());
	if (json_value_get_type(root_v) != JSONObject)
	{
		root_v = json_value_init_object();

		json_object_set_string(json_value_get_object(root_v), "UserID", "initialized");
		
		while (item != list_modules.end() && ret == true)
		{
			item._Ptr->_Myval->SetDefault();
			item._Ptr->_Myval->Save(root_v);
			item++;
		}

		json_serialize_to_file(root_v, "config_data.json");

		
	}
	else
	{
		while (item != list_modules.end() && ret == true)
		{
			ret = item._Ptr->_Myval->Load(root_v);
			item++;
		}
	}

	// Call Init() in all modules
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Init();

		item++;		
	}


	// After all Init calls we call Start() in all modules
	PLOG("Application Start --------------");

	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Start();

		item++;
	}

	json_serialize_to_file(root_v, "config_data.json");
	
	
	return ret;
}



// ---------------------------------------------
void Application::PrepareUpdate()
{
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		item._Ptr->_Myval->PrepareUpdate();

		item++;
	}

	EventSystemBroadcast();
}

// ---------------------------------------------
void Application::FinishUpdate()
{

	/*bool ret = true;

	std::list<Module*>::iterator item = list_modules.begin();

	if(input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
		while (item != list_modules.end() && ret == UPDATE_CONTINUE)
		{
			ret = item._Ptr->_Myval->Save(nullptr);
			item++;
		}

	item = list_modules.begin();

	if(input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		while (item != list_modules.end() && ret == UPDATE_CONTINUE)
		{
			ret = item._Ptr->_Myval->Load(nullptr);
			item++;
		}*/

}



// Call PreUpdate, Update and PostUpdate on all modules
bool Application::PreUpdate()
{
	bool ret = true;

	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret)
	{
		ret = item._Ptr->_Myval->PreEditorUpdate(time->GetDT());

		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret)
	{
		ret = item._Ptr->_Myval->PreUpdate(time->GetDT());

		item++;
	}

	return ret;
}

bool Application::Update()
{
	bool ret = true;
	
	PrepareUpdate();

	ret = PreUpdate();
	if(ret)
		ret = DoUpdate();

	if(ret)
		ret = PostUpdate();	

	FinishUpdate();

	return ret;
}

bool Application::DoUpdate()
{
	bool ret = true;

	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret)
	{
		ret = item._Ptr->_Myval->EditorUpdate(time->GetDT());

		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret)
	{
		ret = item._Ptr->_Myval->Update(time->GetDT());

		item++;
	}

	return ret;
}

bool Application::PostUpdate()
{
	std::list<Module*>::iterator item = list_modules.begin();

	bool ret = true;

	while (item != list_modules.end() && ret)
	{
		ret = item._Ptr->_Myval->PostEditorUpdate(time->GetDT());

		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret)
	{
		ret = item._Ptr->_Myval->PostUpdate(time->GetDT());

		item++;
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::iterator item = list_modules.end();
	item--;
	while(item != list_modules.begin() && ret == true)
	{
		ret = item._Ptr->_Myval->CleanUp();
		item--;
	}
	return ret;
}



void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}



void Application::EventSystemBroadcast()
{
	eventSys->SendBroadcastedEvents();
}

