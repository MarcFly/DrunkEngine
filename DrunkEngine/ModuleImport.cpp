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
	return true;
}

GameObject * ModuleImport::ImportGameObject(const char* path, const aiScene* scene, const aiNode * obj_node, GameObject* par)
{
	GameObject* ret = new GameObject();

	ret->parent = par;
	
	std::string filename = ".\\Library\\";

	if (ret->parent != nullptr)
	{
		ret->root = ret->parent->root;
		ret->par_UUID = ret->parent->UUID;
	}
	else
	{
		ret->root = ret;
		ret->par_UUID = UINT_FAST32_MAX;
	}

	App->gameObj->objects_in_scene.push_back(ret);
	App->gameObj->non_static_objects_in_scene.push_back(ret);

	ret->name = obj_node->mName.C_Str();
	
	// Sequential Import for FBX Only, will create the components one by one
	

	for (int i = 0; i < obj_node->mNumMeshes; i++)
	{
		std::string meshname = filename;
		meshname += GetFileName(path) + "_Mesh_" + std::to_string(obj_node->mMeshes[i]);
		meshname.append(".meshdrnk");
		DGUID fID(GetMD5ID(meshname.c_str()).c_str());
		ComponentMesh* aux = new ComponentMesh(ret);

		if(fID.MD5ID[0] == -52)
			mesh_i->ExportMesh(scene, obj_node->mMeshes[i], path);		

		if (!App->resources->InLibrary(fID))
		{
			MetaMesh* map_mesh = new MetaMesh();
			std::string meta_file = filename + GetFileName(path) + "_Mesh_" + std::to_string(obj_node->mMeshes[i]) + ".meta";
			map_mesh->LoadMetaFile(meta_file.c_str());
			fID = GetMD5ID(meshname.c_str()).c_str();
			App->resources->Library[fID] = map_mesh;
		}

		mesh_i->LinkMesh(fID, aux);

		aux->parent = ret;

		if (aux->mat_ind != -1)
		{
			std::string matname = filename;
			matname += GetFileName(path) + "_Mat_" + std::to_string(i);
			matname.append(".matdrnk");
			DGUID mfID(GetMD5ID(matname.c_str()).c_str());
			matname.clear();
			if(!App->resources->InLibrary(mfID))
			{ 
				MetaMat* map_mat = new MetaMat();
				std::string meta_file = filename + GetFileName(path) + "_Mat_" + std::to_string(aux->mat_ind) + ".meta";
				map_mat->LoadMetaFile(meta_file.c_str());
				mfID = GetMD5ID(meta_file.c_str()).c_str();
				App->resources->Library[mfID] = map_mat;
			}
			ComponentMaterial* aux_mat = new ComponentMaterial(ret);
			mat_i->LinkMat(mfID, aux_mat);
			aux->Material_Ind = mfID;
			ret->components.push_back(aux_mat);
		}

		ret->components.push_back(aux);
		
		meshname.clear();
	}

	for (int i = 0; i < obj_node->mNumChildren; i++)
		ret->children.push_back(ImportGameObject(path, scene, obj_node->mChildren[i], ret));
	ret->GetTransform()->SetFromMatrix(&obj_node->mTransformation);
	App->gameObj->Main_Cam->LookToObj(ret, ret->SetBoundBox());

	return ret;
}

void ModuleImport::LoadSceneData(const char* path, const aiScene* scene)
{
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		DebugTimer.Start();
		std::string matname = ".\\Library\\";
		matname += GetFileName(path) + "_Mat_" + std::to_string(i);
		matname.append(".matdrnk");
		DGUID fID(matname.c_str());
		if (fID.MD5ID[0] == -52)
			mat_i->ExportMat(scene, i, path);

		PLOG("Mat %d export took %d", i, DebugTimer.Read());
	}
}

void ModuleImport::ExportScene(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	std::string aux = file_path;

	if (scene == nullptr)
	{
		// Trying to load it from the scene folder
		std::string new_file_path = file_path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = App->scene->scene_folder + new_file_path;

		scene = aiImportFile(new_file_path.c_str(), aiProcessPreset_TargetRealtime_Fast);
		aux = new_file_path;
	}
	else
		App->ui->console_win->AddLog("Succesful Load from file %s", file_path);

	if (scene->HasMeshes())
		for (int i = 0; i < scene->mNumMeshes; i++)
			mesh_i->ExportMesh(scene, i, aux.c_str());

	if(scene->HasMaterials())
		for(int i = 0; i < scene->mNumMaterials; i++)
			for (int j = 0; j < scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE); j++)
			{
				aiString path;
				scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, j, &path);
				mat_i->ExportTexture(path.C_Str());
			}
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

FileType ModuleImport::CheckExtension(std::string & ext)
{
	FileType ret = FT_Error;

	if (ext == std::string(".fbx") || ext == std::string(".FBX"))
		ret = FT_New_Object;
	else if (ext == std::string(".png") || ext == std::string(".bmp") || ext == std::string(".jpg") || ext == std::string(".dds"))
		ret = FT_Texture;

	return ret;
}

void ModuleImport::LoadFileType(char * file, FileType type)
{
	if (type == FT_New_Object)
	{
		//ExportScene(file);
		App->scene->LoadFBX(file);
	}
	else if (type == FT_Texture)
		mat_i->ExportTexture(file);
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

