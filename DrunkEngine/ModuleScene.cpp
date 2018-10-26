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
	//LoadFromFile("./Test Models and Textures/BakerHouse.fbx");
	//LoadFromFile("./Assets/Ogre.fbx");
	//LoadFromFile("./Test Models and Textures/KSR-29 sniper rifle new_fbx_74_binary.fbx");

	App->renderer3D->OnResize(App->window->window_w, App->window->window_h);

	Save(nullptr);

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

bool ModuleScene::LoadFromFile(const char* file_path)
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
	if (root_value == NULL)
	{
		JSON_Object* obj = json_value_get_object(root_value);
		if (obj == NULL)
		{
			scene_folder = json_object_dotget_string(obj, "manage_mesh.scenes_path");

			std::string default_load = json_object_dotget_string(obj, "manage_mesh.default_load");

			default_load = scene_folder + default_load;

			JSON_Value* scene = json_parse_file(default_load.c_str());
			JSON_Object* scene_obj = json_value_get_object(scene);
			getRootObj()->Load(scene, default_load.c_str());
		}
	}
	ret = true;
	return ret;
}

bool ModuleScene::Save(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	JSON_Object* root_obj = json_value_get_object(root_value);

	// Write Module Scene config data to root_obj

	json_serialize_to_file(root_value, "config_data.json");
	App->ui->console_win->AddLog("ModuleScene config saved");

	if (getRootObj() != nullptr)
	{
		std::string Save_scene = getRootObj()->name + ".json";
		JSON_Value* scene = json_parse_file(Save_scene.c_str());
		if (scene == nullptr)
		{
			scene = json_value_init_object();
			json_serialize_to_file(scene, Save_scene.c_str());
			scene = json_parse_file(Save_scene.c_str());
		}
		getRootObj()->Save(scene, Save_scene.c_str());

		json_serialize_to_file(scene, Save_scene.c_str());

		App->ui->console_win->AddLog("%s Scene saved", Save_scene.c_str());
	}

	ret = true;
	return ret;
}

//-SET OBJ DATA------------------------------------------------------------------------------------------

// -----------------------
int ModuleScene::GetDevILVer()
{
	return IL_VERSION;
}

//void ModuleScene::SetCurrTexTo(GameObject& curr_obj, const int tex_ind)
//{
//	for (int i = 0; i < curr_obj.meshes.size(); i++)
//		curr_obj.meshes[i].tex_index = tex_ind;
//}

// CREATE PRIMITIVE OBJECTS -------------------------------------------------------------------------------
/*
bool ModuleScene::CreatePrimitiveObject(const vec& center, PCube& cube)
{
	bool ret = true;

	GameObject add_obj;

	PCube* new_cube = new PCube(cube);

	new_cube->MathBody = new AABB(Sphere(center, cube.size.x/2.0f));

	ComponentMesh mcube;
	mcube.num_vertex = new_cube->MathBody->NumVertices();
	mcube.vertex = new float[mcube.num_vertex*3];
	memcpy(mcube.vertex, &new_cube->MathBody->CornerPoint(0), 3 * sizeof(float)*mcube.num_vertex);

	std::vector<int> vert_order =
	{	0,3,2,	0,1,3,
		1,5,7,	1,7,3,
		5,4,7,	4,6,7,
		4,0,2,	4,2,6,
		2,3,7,	2,7,6,
		0,4,5,	0,5,1
	};

	mcube.num_index = new_cube->MathBody->NumFaces() * 3;
	mcube.index = new GLuint[mcube.num_index];
	memcpy(mcube.index, &vert_order[0], 3 * sizeof(GLuint)*mcube.num_vertex);

	mcube.num_normal = mcube.num_index * 2;
	mcube.normal = new float[mcube.num_normal];

	for (int i = 0; i < mcube.num_index / 3; i++)
		SetNormals(mcube, i);

	std::vector<float> tex_coords =
	{
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	0,1,0,
		1,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	0,1,0,	1,1,0,
		0,0,0,	0,1,0,	1,1,0

	};

	mcube.num_uvs = tex_coords.size();
	mcube.tex_coords = new float[mcube.num_uvs];
	memcpy(mcube.tex_coords, &tex_coords[0], sizeof(float)*mcube.num_uvs);

	GenBuffers(mcube);

	add_obj.meshes.push_back(mcube);

	if (Objects.size() > 0)
	{
		DestroyObject(0);
		Objects.insert(Objects.begin(), add_obj);
		Objects.pop_back();
	}
	else
		Objects.push_back(add_obj);

	SetParents();

	return ret;
}
*/

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