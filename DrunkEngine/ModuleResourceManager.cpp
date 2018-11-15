#include "ModuleResourceManager.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

ModuleResourceManager::ModuleResourceManager(bool start_enabled) : Module(start_enabled, Type_ResManager)
{

}

bool ModuleResourceManager::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleResourceManager::CleanUp()
{
	bool ret = true;

	return ret;
}

DGUID ModuleResourceManager::AddResource(const char* file)
{
	DGUID ret;

	MetaResource* map_res = NewResource(CheckExtension(GetExtension(file)));
	if (map_res != nullptr && map_res->type != RT_Error)
	{
		std::string metaname = ".\\Library\\" + GetFileName(file) + ".meta";

		map_res->LoadMetaFile(metaname.c_str());
		App->resources->Library[DGUID(file)] = map_res;
		ret = (--Library.end())._Ptr->_Myval.first;
	}
	
	
	return ret;
}

MetaResource* ModuleResourceManager::NewResource(FileType type)
{
	switch (type)
	{
	case FT_Texture:
		return new MetaTexture();
	case FT_Material:
		return new MetaMat();
	case FT_Mesh:
		return new MetaMesh();
	default:
		return nullptr;
	}
}