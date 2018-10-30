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
#include "GeoPropertiesWindow.h"
#include "ModuleWindow.h"
#include "ComponentCamera.h"

#include "ModuleRenderer3D.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "DevIL/libx86/Release/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/Release/ILU.lib")

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
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
	//LoadFromFile("./Assets/BakerHouse.fbx");
	//LoadFromFile("./Assets/Ogre.fbx");
	LoadFBX("./Assets/KSR-29 sniper rifle new_fbx_74_binary.fbx");

	//LoadSceneFile("Scene.json");

	App->renderer3D->OnResize();

	SaveScene();

	return ret;
}

bool ModuleScene::CleanUp()
{
	bool ret = false;

	PLOG("Destroying all objects");

	ret = DestroyScene();

	// detach log streamW
	aiDetachAllLogStreams();

	return ret;
}

bool ModuleScene::LoadFBX(const char* file_path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast);  // for better looks i guess: aiProcessPreset_TargetRealtime_MaxQuality);

	std::string aux = file_path;

	if (scene == nullptr)
	{
		// Trying to load it from the scene folder
		std::string new_file_path = file_path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = scene_folder + new_file_path;

		scene = aiImportFile(new_file_path.c_str(), aiProcessPreset_TargetRealtime_Fast);
		aux = new_file_path;
	}
	else
		App->ui->console_win->AddLog("Failed to Load from file %s", file_path);

	if (scene != nullptr)
	{
		DestroyScene();
		Root_Object = new GameObject(file_path, scene, scene->mRootNode, aux.substr(aux.find_last_of("\\/") + 1).c_str());
		
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
	JSON_Value* scene = json_parse_file(file_path);
	JSON_Object* obj_g = json_value_get_object(scene);
	JSON_Array* gos = json_object_get_array(obj_g, "scene");
	for (int i = 0; i < json_array_get_count(gos); i++)
	{
		JSON_Value* val = json_array_get_value(gos, i);

		getRootObj()->children.push_back(new GameObject());
		getRootObj()->children[getRootObj()->children.size() - 1]->Load(val, file_path);
	}

	OrderScene();

	return true;
}

void ModuleScene::SetActiveFalse()
{
	for (int i = 0; i < active_objects.size(); i++)
	{
		active_objects[i]->active = false;
	}
	active_objects.clear();
}

void ModuleScene::SetmainCam(ComponentCamera * cam)
{
	Main_Cam = cam;
}

bool ModuleScene::Load(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");

	JSON_Object* obj = json_value_get_object(root_value);
		
	scene_folder = json_object_dotget_string(obj, "scene.scenes_path");

	std::string default_load = json_object_dotget_string(obj, "scene.default_load");

	default_load = scene_folder + default_load;

	if (getRootObj() == nullptr)
		NewScene();
			
	ret = true;
	return ret;
}

bool ModuleScene::Save(JSON_Value * root_value)
{
	bool ret = false;

	

	JSON_Object* root_obj = json_value_get_object(root_value);
	// Write Module Scene config data to root_obj
	std::string Save_scene = "";
	if(getRootObj() != nullptr)
		Save_scene = getRootObj()->name + ".drnk";
	json_object_dotset_string(root_obj, "scene.default_load", Save_scene.c_str());
	json_object_dotset_string(root_obj, "scene.scenes_path", scene_folder.c_str());
	json_serialize_to_file(root_value, "config_data.json");

	ret = true;
	return ret;
}

void ModuleScene::SaveScene()
{
	std::string Save_scene = "";

	if (getRootObj() != nullptr)
	{
		Save_scene = getRootObj()->name + ".json";
		JSON_Value* scene = json_parse_file(Save_scene.c_str());
		if (scene == nullptr)
		{
			scene = json_value_init_object();
			json_serialize_to_file(scene, Save_scene.c_str());
			scene = json_parse_file(Save_scene.c_str());
		}
		JSON_Value* set_array = json_value_init_array();
		JSON_Array* go = json_value_get_array(set_array);

		getRootObj()->Save(go);

		JSON_Object* set = json_value_get_object(scene);
		json_object_set_value(set, "scene", set_array);

		json_serialize_to_file(scene, Save_scene.c_str());

		App->ui->console_win->AddLog("%s Scene saved", Save_scene.c_str());
	}
}

void ModuleScene::NewScene()
{
	DeleteScene();

	Root_Object = new GameObject();
	GameObject* MainCam = new GameObject();
	MainCam->name = "Main Camera";
	MainCam->components.push_back(new ComponentCamera(MainCam));
	getRootObj()->children.push_back(MainCam);
}

void ModuleScene::DeleteScene()
{
	if (getRootObj() != nullptr)
		getRootObj()->DestroyThisObject();
	delete Root_Object;
}

//-SET OBJ DATA------------------------------------------------------------------------------------------

// -----------------------
int ModuleScene::GetDevILVer()
{
	return IL_VERSION;
}

bool ModuleScene::DestroyScene()
{
	bool ret = true;

	if (Root_Object != nullptr)
	{
		Root_Object->CleanUp();

		delete Root_Object;
		Root_Object = nullptr;
	}

	active_cameras.clear();

	return ret;
}

void ModuleScene::OrderScene()
{
	for (int i = 0; i < getRootObj()->children.size(); i++)
	{
		if (getRootObj()->children[i]->par_UUID != UINT_FAST32_MAX)
		{
			GameObject* obj = getRootObj()->children[i];
			getRootObj()->children[i]->to_pop = true;
			getRootObj()->AdjustObjects();
			obj->to_pop = false;
			GameObject* get = getRootObj()->GetChild(obj->par_UUID);
			obj->parent = get;
			if (get != nullptr)
				get->children.push_back(obj);
			i--;
		}
	}
}
