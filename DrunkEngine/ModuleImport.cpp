#include "ModuleImport.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "Inspector.h"
#include "ComponentCamera.h"
#include "MeshImport.h"
#include "MaterialImport.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Component.h"
#include "MaterialImport.h"
#include "MeshImport.h"
#include "ModuleResourceManager.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "MD5.h"
#include "PrefabImport.h"

#include <fstream>
#include <iostream>

void CallLog(const char* str, char* usrData);

ModuleImport::ModuleImport(bool start_enabled) : Module(start_enabled, Type_Import)
{

}

bool ModuleImport::Init()
{
	SetDirectories();
	
	mesh_i = new MeshImport();
	mat_i = new MatImport();
	prefab_i = new PrefabImport();

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = CallLog;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleImport::CleanUp()
{
	delete mesh_i;
	delete mat_i;
	delete prefab_i;

	return true;
}

void ModuleImport::LoadScene(const char* path)
{
	GameObject* par = App->gameObj->getRootObj();

	if (App->gameObj->getRootObj() == nullptr)
	{
		App->gameObj->CleanUp();
		App->gameObj->NewScene();
		par = App->gameObj->getRootObj();
	}
	else if(App->ui->inspector->selected_object != nullptr)
	{
		par = App->ui->inspector->selected_object;
	}

	JSON_Value* scene = json_parse_file(path);
	JSON_Object* obj_g = json_value_get_object(scene);
	JSON_Array* obj_arr = json_object_get_array(obj_g, "scene");

	for (int i = 0; i < json_array_get_count(obj_arr); i++)
	{
		JSON_Value* val = json_array_get_value(obj_arr, i);

		par->children.push_back(prefab_i->ImportGameObject(path, val));
	}

	par->OrderChildren();	
}

void ModuleImport::ExportScene(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_Fast);

	DebugTimer.Start();

	if (scene == nullptr)
	{
		// Trying to load it from the scene folder
		std::string new_file_path = path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = App->scene->scene_folder + new_file_path;

		scene = aiImportFile(new_file_path.c_str(), aiProcessPreset_TargetRealtime_Fast);
		if (scene == nullptr)
			App->ui->console_win->AddLog("Failed to Load from file %s", path);
	}

	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* mat = scene->mMaterials[i];
		std::string matname = ".\\Library\\";
		matname += GetFileName(path) + "_Mat_" + std::to_string(i);
		matname.append(".matdrnk");
		DGUID fID(matname.c_str());
		if (fID.MD5ID[0] == -52)
			mat_i->ExportAIMat(mat, i, path);
	}

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		std::string meshname = ".\\Library\\";
		meshname += GetFileName(path) + "_Mesh_" + std::to_string(i);
		meshname.append(".meshdrnk");
		DGUID fID(meshname.c_str());
		if (fID.MD5ID[0] == -52)
			mesh_i->ExportAIMesh(mesh, i, path);
	}

	{
		std::string scenename = ".\\Library\\";
		scenename += GetFileName(path) + ".scenedrnk";
		ExportSceneNodes(scenename.c_str(), scene->mRootNode, scene);
	}

	//scene->mNumAnimations
	//scene->numCameras
	//scene->mNumLights
}

void ModuleImport::ExportSceneNodes(const char* path, const aiNode* root_node, const aiScene* aiscene)
{
	JSON_Value* scene = json_parse_file(path);
	if (scene == nullptr)
	{
		scene = json_value_init_object();
		json_serialize_to_file(scene, path);
		scene = json_parse_file(path);
	}
	JSON_Value* set_array = json_value_init_array();
	JSON_Array* go = json_value_get_array(set_array);

	prefab_i->ExportAINode(aiscene, root_node, go, UINT_FAST32_MAX, GetFileName(path).c_str());

	JSON_Object* set = json_value_get_object(scene);
	json_object_set_value(set, "scene", set_array);
	json_serialize_to_file(scene, path);

	App->ui->console_win->AddLog("%s Scene exported", path);
}

void ModuleImport::LoadFile(char * file)
{
	SDL_ShowSimpleMessageBox(
		SDL_MESSAGEBOX_INFORMATION,
		"File dropped on window",
		file,
		App->window->window);

	std::string extension = strrchr(file, '.');

	FileType check = CheckExtension(extension);

	LoadFileType(file, check);

}



void ModuleImport::LoadFileType(char * file, FileType type)
{
	if (type == FT_New_Object)
	{
		ExportScene(file);
	}
	else if (type == FT_Texture)
		mat_i->ExportILTexture(file);
	else if(type == FT_Error)
		App->ui->console_win->AddLog("File format not recognized!\n");
	else
		App->ui->console_win->AddLog("Wtf did you drop?\n");
}

void CallLog(const char* str, char* usrData)
{
	App->ui->console_win->AddLog(str);
}

// -----------------

