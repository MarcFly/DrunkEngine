#include "ResourceManager.h"
#include "Application.h"

ResourceManager::ResourceManager(bool start_enabled) : Module(start_enabled, Type_ResManager)
{

}

bool ResourceManager::Init()
{
	bool ret = true;

	return ret;
}

bool ResourceManager::CleanUp()
{
	bool ret = true;

	return ret;
}