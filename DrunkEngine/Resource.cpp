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

bool DGUID::TrueComp(const DGUID cmp_id) const
{
	bool ret = false;

	std::vector<uint> f = TrueSum();
	std::vector<uint> s = cmp_id.TrueSum();

	for (int i = 0; i < f.size(); i++)
	{
		if (f[i] != s[i])
		{
			ret = (f[i] > s[i]);
			break;
		}
	}


	return ret;
}

std::vector<uint> DGUID::TrueSum() const
{
	std::vector<uint> ret;
	for (int i = 0; i < 16; i++)
	{
		if (i == 0)
			ret.push_back(MD5ID[i] + 1000 * MD5ID[i + 1]);
		else
		{
			ret.push_back(MD5ID[i] + 1000 * MD5ID[i + 1] + 1000000 * MD5ID[i + 2]);
			i++;
		}

		i++;
	}

	return ret;
}