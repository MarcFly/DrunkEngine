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
	Camera_Modified,
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
			int x = -1, y = -1;
		} point2d;

		struct
		{
			GameObject * ptr;
		} game_object;
	};

	enum UnionUsed
	{
		UseString,
		UsePoint2d,
		UseGameObject,
		UseNull

	} unionSelector;

	Event(EventType type, Event::UnionUsed union_to_use) { this->type = type; unionSelector = union_to_use; }
	~Event()
	{
		switch (unionSelector)
		{
		case Event::UseString:
			delete string.ptr;
		case Event::UseGameObject:
			game_object.ptr = nullptr;
		}
	}
};

struct Subscriptor
{
	std::vector<EventType> sub_events;
	Module * module;

	~Subscriptor()
	{
		module = nullptr;
		sub_events.clear();
	}
};

class ModuleEventSystem : public Module
{
public:
	ModuleEventSystem(bool start_enabled = true);
	~ModuleEventSystem();

	void Clear();

	void BroadcastEvent(const Event & event);
	void SendBroadcastedEvents();
	void Subscribe(EventType event, Module * module);

private:
	std::vector<Subscriptor*> subscriptors;
	std::vector<const Event*> queue_of_events;
};

#endif // !_MODULE_EVENTSYSTEM_H_