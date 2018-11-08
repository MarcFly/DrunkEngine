#ifndef _MODULE_GAMEOBJECT_H_
#define _MODULE_GAMEOBJECT_H_

#include "Module.h"

class ModuleGameObject : public Module
{
public:
	ModuleGameObject(bool start_enabled = true);
	~ModuleGameObject();

	void Clear();

private:
	
};

#endif // !_MODULE_EVENTSYSTEM_H_