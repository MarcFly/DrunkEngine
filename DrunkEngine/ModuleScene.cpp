#include "ModuleScene.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "ModuleCamera3D.h"
#include "SDL/include/SDL_opengl.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "Inspector.h"
#include "ModuleWindow.h"
#include "ComponentCamera.h"
#include "ModuleRenderer3D.h"
#include "FileHelpers.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "DevIL/libx86/Release/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/Release/ILU.lib")

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled, Type_Scene)
{

}

bool ModuleScene::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleScene::Start()
{
	bool ret = true;

	//Load(nullptr);
	LoadFBX("./Assets/Street environment_V01.FBX");
	//LoadFBX("./Assets/Ogre.fbx");
	//LoadFBX("./Assets/KSR-29 sniper rifle new_fbx_74_binary.fbx");
	//LoadFBX("./Assets/Cube3d.fbx");
	//LoadSceneFile("Scene.json");

	App->renderer3D->OnResize();

	//SaveScene();

	App->eventSys->Subscribe(EventType::Window_Resize, this);
	App->eventSys->Subscribe(EventType::Camera_Modified, this);

	return ret;
}

bool ModuleScene::CleanUp()
{
	bool ret = false;

	PLOG("Destroying all objects");

	ret = App->gameObj->CleanUp();

	// detach log streamW
	aiDetachAllLogStreams();

	return ret;
}

bool ModuleScene::LoadFBX(const char* file_path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast); 

	std::string aux = file_path;

	if (scene == nullptr)
	{
		// Trying to load it from the scene folder
		std::string new_file_path = file_path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = scene_folder + new_file_path;

		scene = aiImportFile(new_file_path.c_str(), aiProcessPreset_TargetRealtime_Fast);
		aux = new_file_path;
		if(scene == nullptr)
			App->ui->console_win->AddLog("Failed to Load from file %s", file_path);
	}
		

	if (scene != nullptr)
	{
		if (App->gameObj->getRootObj() == nullptr)
			App->gameObj->NewScene();
		
		if (App->ui->inspector->selected_object != nullptr)
			App->ui->inspector->selected_object->children.push_back(new GameObject(file_path, scene, scene->mRootNode, aux.substr(aux.find_last_of("\\/") + 1).c_str(), App->ui->inspector->selected_object));
		else
			App->gameObj->getRootObj()->children.push_back(new GameObject(file_path, scene, scene->mRootNode, aux.substr(aux.find_last_of("\\/") + 1).c_str(), App->gameObj->getRootObj()));
		
		App->gameObj->CreateMainCam();

		aiReleaseImport(scene);
	}
	else
	{
		App->ui->console_win->AddLog("Error loading scene's meshes %s", file_path);
		ret = false;
	}

	return ret;
}

bool ModuleScene::LoadSceneFile(const char* file_path)
{
	App->gameObj->CleanUp();
	App->gameObj->NewScene();
	JSON_Value* scene = json_parse_file(file_path);
	JSON_Object* obj_g = json_value_get_object(scene);
	JSON_Array* gos = json_object_get_array(obj_g, "scene");
	for (int i = 0; i < json_array_get_count(gos); i++)
	{
		JSON_Value* val = json_array_get_value(gos, i);

		App->gameObj->getRootObj()->children.push_back(new GameObject());
		App->gameObj->getRootObj()->children[App->gameObj->getRootObj()->children.size() - 1]->Load(val, file_path);
	}

	OrderScene();

	return true;
}

bool ModuleScene::Load(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");

	JSON_Object* obj = json_value_get_object(root_value);
		
	scene_folder = json_object_dotget_string(obj, "scene.scenes_path");

	std::string default_load = json_object_dotget_string(obj, "scene.default_load");

	default_load = scene_folder + default_load;
			
	ret = true;
	return ret;
}

bool ModuleScene::Save(JSON_Value * root_value)
{
	bool ret = false;	

	JSON_Object* root_obj = json_value_get_object(root_value);
	// Write Module Scene config data to root_obj
	std::string Save_scene = "";
	if(App->gameObj->getRootObj() != nullptr)
		Save_scene = App->gameObj->getRootObj()->name + ".drnk";
	json_object_dotset_string(root_obj, "scene.default_load", Save_scene.c_str());
	json_object_dotset_string(root_obj, "scene.scenes_path", scene_folder.c_str());
	json_serialize_to_file(root_value, "config_data.json");

	ret = true;
	return ret;
}

void ModuleScene::SaveScene(const char* filename)
{
	App->gameObj->getRootObj()->name = GetFileName(filename);

	if (App->gameObj->getRootObj() != nullptr)
	{
		std::string Save_scene = App->gameObj->getRootObj()->name + ".drnk";
		JSON_Value* scene = json_parse_file(Save_scene.c_str());
		if (scene == nullptr)
		{
			scene = json_value_init_object();
			json_serialize_to_file(scene, Save_scene.c_str());
			scene = json_parse_file(Save_scene.c_str());
		}
		JSON_Value* set_array = json_value_init_array();
		JSON_Array* go = json_value_get_array(set_array);

		App->gameObj->getRootObj()->Save(go);

		JSON_Object* set = json_value_get_object(scene);
		json_object_set_value(set, "scene", set_array);

		json_serialize_to_file(scene, Save_scene.c_str());

		App->ui->console_win->AddLog("%s Scene saved", Save_scene.c_str());
	}
}

//-SET OBJ DATA------------------------------------------------------------------------------------------

// -----------------------
int ModuleScene::GetDevILVer()
{
	return IL_VERSION;
}

void ModuleScene::OrderScene()
{
	for (int i = 0; i < App->gameObj->getRootObj()->children.size(); i++)
	{
		if (App->gameObj->getRootObj()->children[i]->par_UUID != UINT_FAST32_MAX)
		{
			GameObject* obj = App->gameObj->getRootObj()->children[i];
			App->gameObj->getRootObj()->children[i]->to_pop = true;
			App->gameObj->getRootObj()->AdjustObjects();
			obj->to_pop = false;
			GameObject* get = App->gameObj->getRootObj()->GetChild(obj->par_UUID);
			obj->parent = get;
			if (get != nullptr)
				get->children.push_back(obj);
			i--;
		}
	}
}

void ModuleScene::RecieveEvent(const Event & event)
{
	switch (event.type)
	{
	case EventType::Camera_Modified:
	{
		break;
	}
	case EventType::Window_Resize:
	{
		break;
	}
	default:
		break;
	}
}
