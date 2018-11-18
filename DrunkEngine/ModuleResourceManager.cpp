#include "ModuleResourceManager.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include <map>
ModuleResourceManager::ModuleResourceManager(bool start_enabled) : Module(start_enabled, Type_ResManager)
{

}

bool ModuleResourceManager::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleResourceManager::PostUpdate(float dt)
{
	std::map<DGUID, MetaResource*>::iterator it = (Library.begin());
	for (int i = 0; i < Library.size(); i++, it++)
	{
		MetaResource* item = it._Ptr->_Myval.second;

		//item->Asset.par = item;

		if (item->Asset.IsInUse())
			item->Asset.UnloadFromMem();
	}

	return true;
}

bool ModuleResourceManager::CleanUp()
{
	bool ret = true;

	std::map<DGUID, MetaResource*>::iterator it = (Library.begin());
	
	for (int i = 0; i < Library.size(); i++, it++)
	{
		MetaResource* item = it._Ptr->_Myval.second;

		if(item->Asset.IsLoaded())
			item->Asset.UnloadFromMem();
		
		delete it._Ptr->_Myval.second;
		it._Ptr->_Myval.second = nullptr;
	}

	Library.clear();

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
		ret = DGUID(file);
		Library[ret] = map_res;
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