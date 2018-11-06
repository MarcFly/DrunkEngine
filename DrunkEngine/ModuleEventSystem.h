#ifndef _MODULEEVENTSYSTEM_H_
#define _MODULEEVENTSYSTEM_H_

#include "Module.h"
#include "Globals.h"

enum EventType
{
	GameObject_Detroyed,
	Play,
	Pause,
	Unpause,
	Stop,
	Window_Resize,
	File_Dropped,
	Transform_Updated,
	NULL_Event
};

struct Event
{
	EventType type;

	union
	{
		struct
		{
			const char* ptr;
		} string;

		struct
		{
			int x, y;
		} pint2d;
	};

	Event(EventType type) : type(type) {}

};

class ModuleEventSystem : public Module
{
public:
	ModuleEventSystem(bool start_enabled = true);
	~ModuleEventSystem();

	bool Start();
	//update_status Update(float dt);
	//bool CleanUp();
	//
	//bool Load(JSON_Value* root_value);
	//bool Save(JSON_Value* root_value);
}

#endif // !_MODULEEVENTSYSTEM_H_