#ifndef _MODULE_H_
#define _MODULE_H_

#include "parson/parson.h"
#include <list>
#include "Globals.h"
#include "Timer.h"

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
	Type_GameObj,
	Type_ResManager,
	Type_Time,
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

	virtual void PrepareUpdate()
	{
	}

	virtual bool PreUpdate(float game_dt)
	{
		return true;
	}

	virtual bool PreEditorUpdate(float real_dt)
	{
		return true;
	}

	virtual bool Update(float game_dt)
	{
		return true;
	}

	virtual bool EditorUpdate(float real_dt)
	{
		return true;
	}

	virtual bool PostUpdate(float game_dt)
	{
		return true;
	}

	virtual bool PostEditorUpdate(float real_dt)
	{
		return true;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2) {}

	virtual bool Load(const JSON_Value* root_value)
	{
		return true; 
	}

	virtual bool Save(JSON_Value* root_value)
	{
		return true; 
	}

	virtual void SetDefault() {};

	virtual TypeModule GetType() const
	{
		return type_m;
	}

	virtual void RecieveEvent(const Event & event) {}

public:
	Timer DebugTimer;
};

#endif // !_MODULE_H_