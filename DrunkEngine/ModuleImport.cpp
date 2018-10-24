#include "ModuleImport.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"
#include "ComponentCamera.h"
#include "MeshImport.h"
#include "MaterialImport.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

void CallLog(const char* str, char* usrData);

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
	ret->root = ret->parent;
	while (ret->root->parent != nullptr)
		ret->root = ret->root->parent;

	ret->name = obj_node->mName.C_Str();

	//ret->SetOriginalLoadPath(path);

	for (int i = 0; i < obj_node->mNumMeshes; i++)
	{
		std::string filename = "./Library/Meshes/";
		filename += GetFileName(path) + "_Mesh_" + std::to_string(obj_node->mMeshes[i]);
		filename.append(".meshdrnk");
		ComponentMesh* aux = mesh_i->ImportMesh(filename.c_str(), ret);
		if (aux == nullptr)
		{
			mesh_i->ExportMesh(scene, obj_node->mMeshes[i],path);
			mesh_i->ImportMesh(filename.c_str(), ret);
		}
		
	}
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		std::string filename = "./Library/Materials/";
		filename += GetFileName(path) + "_Mat_" + std::to_string(i);
		filename.append(".matdrnk");
		ComponentMaterial* aux = mat_i->ImportMat(filename.c_str(), ret);
		if (aux == nullptr)
		{
			mat_i->ExportMat(scene, i, path);
			aux = mat_i->ImportMat(filename.c_str(), ret);
		}
		ret->materials.push_back(aux);
	}

	for (int i = 0; i < obj_node->mNumChildren; i++)
		ret->children.push_back(ImportGameObject(path, scene, obj_node->mChildren[i], ret));

	ret->transform = new ComponentTransform(&obj_node->mTransformation, ret);
	App->mesh_loader->Main_Cam->LookToObj(ret, ret->SetBoundBox());

	return ret;
}


void ModuleImport::ExportScene(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast);// for better looks i guess: aiProcessPreset_TargetRealtime_MaxQuality);
	std::string aux = file_path;

	if (scene == nullptr)
	{
		// Trying to load it from the scene folder
		std::string new_file_path = file_path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = App->mesh_loader->scene_folder + new_file_path;

		scene = aiImportFile(new_file_path.c_str(), aiProcessPreset_TargetRealtime_Fast);
		aux = new_file_path;
	}
	else
		App->ui->console_win->AddLog("Failed to Load from file %s", file_path);

	if (scene->HasMeshes())
		for (int i = 0; i < scene->mNumMeshes; i++)
			mesh_i->ExportMesh(scene, i, file_path);
	if(scene->HasMaterials())
		for(int i = 0; i < scene->mNumMaterials; i++)
			for (int j = 0; j < scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE); j++)
			{
				aiString path;
				aiReturn err = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, j, &path);
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
		ExportScene(file);
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

