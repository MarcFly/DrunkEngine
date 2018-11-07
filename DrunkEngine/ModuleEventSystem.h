#ifndef _MODULE_EVENTSYSTEM_H_
#define _MODULE_EVENTSYSTEM_H_

#include "Module.h"

class GameObject;

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

		struct
		{
			GameObject * ptr
		} game_object;
	};

	Event(EventType type) : type(type) {}
};

struct Subscriptor
{
	std::vector<EventType> sub_events;
	Module * module;
};

class ModuleEventSystem : public Module
{
public:
	ModuleEventSystem(bool start_enabled = true);
	~ModuleEventSystem();

	void BroadcastEvent(const Event & event);
	void SendBroadcastedEvents();
	void Subscribe(EventType event, Module * module);

private:
	std::vector<Subscriptor*> subscriptors;
	std::vector<const Event*> queue_of_events;
};

#endif // !_MODULE_EVENTSYSTEM_H_