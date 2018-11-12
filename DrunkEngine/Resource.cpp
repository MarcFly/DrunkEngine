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
	case RT_Mesh:
		mesh.ptr = new ResourceMesh();
		mesh.ptr = App->importer->mesh_i->LoadMesh(par->file.c_str());
		break;
	case RT_Material:
		mat.ptr = new ResourceMaterial();
		mat.ptr = App->importer->mat_i->LoadMat(par->file.c_str());
		break;
	case RT_Texture:
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
	}
}

bool Resource::IsLoaded()
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
	}
}
//--------------------------------------
