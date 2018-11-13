#include "Resource.h"
#include "ModuleResourceManager.h"
#include "Application.h"
#include "MeshImport.h"
#include "MaterialImport.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

void Resource::LoadToMem()
{
	switch (par->type)
	{
	case RT_GameObject:
		break;
	case RT_Mesh:
		mesh.ptr = App->importer->mesh_i->LoadMesh(par->file.c_str());
		break;
	case RT_Material:
		mat.ptr = App->importer->mat_i->LoadMat(par->file.c_str());
		break;
	case RT_Texture:
		texture.ptr = App->importer->mat_i->LoadTexture(par->file.c_str());
		break;
	default:
		break;
	}
}

void Resource::UnloadFromMem()
{
	switch (par->type)
	{
	case RT_GameObject:
	case RT_Mesh:
		
	case RT_Material:
	case RT_Texture:
		break;
	default:
		break;
	}
}

bool Resource::IsLoaded()
{
	if (par->UseCount > 0)
	{
		switch (par->type)
		{
		case RT_GameObject:
			break;
		case RT_Mesh:
			return (mesh.ptr != nullptr);
		case RT_Material:
			return (mat.ptr != nullptr);
		case RT_Texture:
			return (texture.ptr != nullptr);
		default:
			break;
		}
	}
	return false;
}

//--------------------------------------
