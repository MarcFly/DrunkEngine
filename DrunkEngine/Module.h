#ifndef _MODULE_H_
#define _MODULE_H_

#include "parson/parson.h"
#include <list>
#include "Globals.h"

class Application;
struct PhysBody3D;

class Module
{
private :
	bool enabled;

public:

	Module(bool start_enabled = true)
	{}

	virtual ~Module()
	{}

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

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}

	virtual bool Load(JSON_Value* root_value)
	{
		return UPDATE_CONTINUE; 
	}

	virtual bool Save(JSON_Value* root_value)
	{
		return UPDATE_CONTINUE; 
	}

	virtual void SetDefault() {};

};

#endif // !_MODULE_H_