#include "ModuleEventSystem.h"
#include "Application.h"

ModuleEventSystem::ModuleEventSystem(bool start_enabled) : Module(start_enabled)
{

}

ModuleEventSystem::~ModuleEventSystem()
{
}

bool ModuleEventSystem::Start()
{
	return true;
}
