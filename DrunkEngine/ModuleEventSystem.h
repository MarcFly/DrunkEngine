#ifndef _MODULE_EVENTSYSTEM_H_
#define _MODULE_EVENTSYSTEM_H_

#include "Module.h"

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

public:

};

#endif // !_MODULE_EVENTSYSTEM_H_