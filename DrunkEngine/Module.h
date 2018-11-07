#ifndef _MODULE_H_
#define _MODULE_H_

#include "parson/parson.h"
#include <list>
#include "Globals.h"

class Application;
struct PhysBody3D;
struct Event;

enum TypeModule
{
	Type_Camera3D,
	Type_EventSys,
	Type_Input,
	Type_Render3D,
	Type_Scene,
	Type_Window,
	Type_UI,
	Type_Import,
	Type_Null
};

class Module
{
private :
	bool enabled;
	TypeModule type_m;

public:

	Module(bool start_enabled, TypeModule type) { start_enabled = true; type_m = type; }

	virtual ~Module() {}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2) {}

	virtual bool Load(JSON_Value* root_value)
	{
		return UPDATE_CONTINUE; 
	}

	virtual bool Save(JSON_Value* root_value)
	{
		return UPDATE_CONTINUE; 
	}

	virtual void SetDefault() {};

	virtual TypeModule GetType() const
	{
		return type_m;
	}

	virtual void RecieveEvent(const Event & event) {}
};

#endif // !_MODULE_H_