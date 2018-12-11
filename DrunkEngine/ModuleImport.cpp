#include "ModuleImport.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "SceneViewerWindow.h"
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
#include "SkeletonImport.h"
#include "AnimationImport.h"

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
	skel_i = new SkeletonImport();
	anim_i = new AnimationImport();

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
	delete skel_i;
	delete anim_i;

	return true;
}

void ModuleImport::LoadScene(const char* path)
{
	JSON_Value* scene = json_parse_file(path);
	
	JSON_Object* obj_g = json_value_get_object(scene);
	JSON_Array* obj_arr = json_object_get_array(obj_g, "scene");

	GameObject* par = App->gameObj->getRootObj();

	if (App->gameObj->getRootObj() == nullptr)
	{
		App->gameObj->NewScene();
		par = App->gameObj->getRootObj();
	}
	else if (App->ui->scene_viewer_window->selected_object != nullptr)
	{
		par = App->ui->scene_viewer_window->selected_object;
	}

	if (scene != nullptr)
	{
		{
			GameObject* add = prefab_i->ImportGameObject(path, json_array_get_value(obj_arr, 0));
			add->parent = par;

			par->children.push_back(add);
			par = add;
		}

		for (int i = 1; i < json_array_get_count(obj_arr); i++)
		{
			JSON_Value* val = json_array_get_value(obj_arr, i);

			GameObject* add = prefab_i->ImportGameObject(path, val);
			add->parent = par;

			par->children.push_back(add);
		}

		par->OrderChildren();
		par->RecursiveSetNewUUID();

		Event evTrans(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
		evTrans.game_object.ptr = par;
		App->eventSys->BroadcastEvent(evTrans);

		Event evCam(EventType::Update_Cam_Focus, Event::UnionUsed::UseGameObject);
		evCam.game_object.ptr = par;
		App->eventSys->BroadcastEvent(evCam);
	}
}


void ModuleImport::ExportScene(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_Fast);

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

	if (scene != nullptr)
	{
		// Export Materials
		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* mat = scene->mMaterials[i];
			std::string matname = ".\\Library\\";
			matname += GetFileName(path) + "_Mat_" + std::to_string(i);
			matname.append(".matdrnk");
			DGUID fID(matname.c_str());
			if (!fID.CheckValidity())
				mat_i->ExportAIMat(mat, i, path);
		}

		// Export Meshes
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			std::string meshname = ".\\Library\\";
			meshname += GetFileName(path) + "_Mesh_" + std::to_string(i);
			meshname.append(".meshdrnk");
			DGUID fID(meshname.c_str());
			if (!fID.CheckValidity())
			{
				mesh_i->ExportAIMesh(mesh, i, path);
			}
		}
		
		// Export Skeletons
		// I don't know how to check if the skeleton is written
		// As it has to be reconstructed before being able to export it
		std::vector<const aiNode*> NodesWithSkeleton;
		{
			std::vector<const aiNode*> BoneNodes;

			skel_i->FindBoneNodes(scene->mMeshes, scene->mRootNode, BoneNodes);
			skel_i->GetSkeletonRoots(BoneNodes, NodesWithSkeleton);

			std::vector<std::multimap<uint, BoneCrumb*>> Skeletons = skel_i->ReconstructSkeletons(scene, NodesWithSkeleton, BoneNodes);

			for (int i = 0; i < Skeletons.size(); i++)
			{
				skel_i->ExportMapSkeleton(scene, NodesWithSkeleton[i], Skeletons[i], i, scene->mRootNode, path);
			}
		}

		// Export Animations
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* anim = scene->mAnimations[i];
			std::string animname = ".\\Library\\";
			animname += GetFileName(path) + "_Anim_" + std::to_string(i);
			animname.append(".animdrnk");
			DGUID fID(animname.c_str());
			if (!fID.CheckValidity())
				anim_i->ExportAIAnimation(anim, i, path);
		}

		// Export Scene as Prefab
		{
			std::string scenename = ".\\Library\\";
			scenename += GetFileName(path) + ".prefabdrnk";
			ExportSceneNodes(scenename.c_str(), NodesWithSkeleton, scene->mRootNode, scene);
		}
	}

	//scene->mNumAnimations
	//scene->numCameras
	//scene->mNumLights
}

void ModuleImport::ExportSceneNodes(const char* path, std::vector<const aiNode*>& NodesWithSkeleton, const aiNode* root_node, const aiScene* aiscene)
{
	JSON_Value* scene = json_parse_file(path);
	
	JSON_Value* set_array = json_value_init_array();
	JSON_Array* go = json_value_get_array(set_array);

	prefab_i->ExportAINode(aiscene, NodesWithSkeleton, root_node, go, UINT_FAST32_MAX, GetFileName(path).c_str());

	JSON_Object* set = json_value_get_object(scene = json_value_init_object());
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
	if (type == FT_FBX)
		App->scene->LoadFBX(file);
	else if (type == FT_Texture)
		mat_i->ExportILTexture(file);
	else if(type == FT_Error)
		App->ui->console_win->AddLog("File format not recognized!\n");
	else
		App->ui->console_win->AddLog("What did you drop?\n");
}

void CallLog(const char* str, char* usrData)
{
	App->ui->console_win->AddLog(str);
}

// -----------------

