#include "Application.h"
#include "ModuleGameObject.h"

ModuleGameObject::ModuleGameObject(bool start_enabled) : Module(start_enabled, Type_GameObj)
{
}

ModuleGameObject::~ModuleGameObject()
{
}