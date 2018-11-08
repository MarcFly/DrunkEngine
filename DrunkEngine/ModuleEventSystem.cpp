#include "ModuleEventSystem.h"
#include "Application.h"

ModuleEventSystem::ModuleEventSystem(bool start_enabled) : Module(start_enabled, Type_EventSys)
{	
}

ModuleEventSystem::~ModuleEventSystem()
{
}

void ModuleEventSystem::Clear()
{
	for (int i = 0; i < subscriptors.size(); i++)
		delete subscriptors[i];

	subscriptors.clear();

	for (int i = 0; i < queue_of_events.size(); i++)
		delete queue_of_events[i];

	queue_of_events.clear();
}

void ModuleEventSystem::BroadcastEvent(const Event & event)
{
	Event * ev = new Event(event.type, event.unionSelector);
	
	switch (event.unionSelector)
	{
	case Event::UseString:
		ev->string = event.string;
	case Event::UsePoint2d:
		ev->point2d = event.point2d;
	case Event::UseGameObject:
		ev->game_object = event.game_object;
	default:
		break;
	}

	queue_of_events.push_back(ev);
}

void ModuleEventSystem::SendBroadcastedEvents()
{
	for (int i = 0; i < queue_of_events.size(); i++)	//For each event
	{
		for (int j = 0; j < subscriptors.size(); j++)	//For each subsriber
		{
			for (int k = 0; k < subscriptors[j]->sub_events.size(); k++)	//For each event subscription
			{
				if (subscriptors[j]->sub_events[k] == queue_of_events[i]->type)
					subscriptors[j]->module->RecieveEvent(*queue_of_events[i]);
			}
		}
	}

	for (int i = 0; i < queue_of_events.size(); i++)
		delete queue_of_events[i];

	queue_of_events.clear();
}

void ModuleEventSystem::Subscribe(EventType event_type, Module * my_module)
{
	bool is_subscribed = false;

	for (int i = 0; i < subscriptors.size(); i++)
	{
		if (subscriptors[i]->module->GetType() == my_module->GetType())		//Check if the module is already subscribed to an Event
		{
			for (int j = 0; j < subscriptors[i]->sub_events.size(); j++)	//Check that the module is not already subscribed to the same Event
			{
				if (subscriptors[i]->sub_events[j] != event_type)
					;
				else
					return;
			}

			subscriptors[i]->sub_events.push_back(event_type);
			return;
		}
	}

	Subscriptor * sub = new Subscriptor;
	sub->module = my_module;
	sub->sub_events.push_back(event_type);
	subscriptors.push_back(sub);
}