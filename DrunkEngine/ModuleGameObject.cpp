#include "Application.h"
#include "ModuleGameObject.h"

ModuleGameObject::ModuleGameObject(bool start_enabled) : Module(start_enabled, Type_GameObj)
{
}

ModuleGameObject::~ModuleGameObject()
{
}

update_status ModuleGameObject::Update(float dt)
{
	if (Root_Object != nullptr)
		Root_Object->Update(dt);


	return UPDATE_CONTINUE;
}

bool ModuleGameObject::CleanUp()
{
	bool ret = true;

	return ret;
}

void ModuleGameObject::Draw()
{

}

void ModuleGameObject::SetActiveFalse()
{
	for (int i = 0; i < active_objects.size(); i++)
	{
		active_objects[i]->active = false;
	}
	active_objects.clear();
}

void ModuleGameObject::SetmainCam(ComponentCamera * cam)
{
	Main_Cam = cam;
}

void ModuleGameObject::NewScene()
{
	DeleteScene();

	Root_Object = new GameObject();
	Root_Object->name = "NewScene";
}

void ModuleGameObject::CreateMainCam()
{
	if (active_cameras.size() < 1)
	{
		GameObject* MainCam = new GameObject(Root_Object, "Main Camera", CT_Camera);
		getRootObj()->children.push_back(MainCam);
	}
}

void ModuleGameObject::DeleteScene()
{
	if (getRootObj() != nullptr)
		getRootObj()->DestroyThisObject();
	delete Root_Object;
}