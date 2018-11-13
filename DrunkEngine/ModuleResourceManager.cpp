#include "ModuleResourceManager.h"
#include "Application.h"

ModuleResourceManager::ModuleResourceManager(bool start_enabled) : Module(start_enabled, Type_ResManager)
{

}

bool ModuleResourceManager::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleResourceManager::CleanUp()
{
	bool ret = true;

	return ret;
}