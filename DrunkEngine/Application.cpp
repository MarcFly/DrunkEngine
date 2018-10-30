#include "Application.h"
#include "ConsoleWindow.h"
#include "parson/parson.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	//physics = new ModulePhysics3D(this);
	importer = new ModuleImport();
	ui = new ModuleUI(this);
	mesh_loader = new ModuleScene(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	//AddModule(physics);
	AddModule(importer);
	AddModule(mesh_loader);
	AddModule(ui);

	// Renderer last!
	AddModule(renderer3D);

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
		item._Ptr->_Myval->Save(root_v);
		item++;
	}

	json_serialize_to_file(root_v, "config_data.json");

	
	fps_timer.Start();
	ms_timer.Start();
	count_fps = 0;
	fps = 0;
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	Frame_Metrics();
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
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->PreUpdate(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->Update(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->PostUpdate(dt);
		item++;
	}

	FinishUpdate();
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

float Application::GetFPS()
{
	return 1.0f / dt;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::Frame_Metrics()
{
	//dt

	// Very badly constructed delay in order to cap fps
	/*if (dt > 0 && fps_cap > 0 && (dt < 1.0f / (float)fps_cap))
		SDL_Delay(1000*((1.0f / (float)fps_cap) - dt));*/

	dt = (float)ms_timer.Read() / 1000.0f;

	ms_timer.Start();

}

void Application::Cap_FPS(const int& cap) 
{
	fps_cap = cap;
}

float Application::GetDt()
{
	return 1000 * dt;
}