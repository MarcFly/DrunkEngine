#include "Resource.h"
#include "ModuleResourceManager.h"
#include "Application.h"
#include "MeshImport.h"
#include "MaterialImport.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceSkeleton.h"

void Resource::LoadToMem()
{
	switch (bs.par->type)
	{
	case RT_Prefab:
		break;
	case RT_Mesh:
		mesh.ptr = App->importer->mesh_i->LoadMesh(bs.par->file.c_str());
		break;
	case RT_Material:
		mat.ptr = App->importer->mat_i->LoadMat(bs.par->file.c_str());
		break;
	case RT_Texture:
		texture.ptr = App->importer->mat_i->LoadTexture(bs.par->file.c_str());
		break;
	case RT_Billboard:
		//billboard.ptr = App->importer->mat_i->LoadTexture(bs.par->file.c_str());
		break;
	case RT_Skeleton:
		skeleton.ptr = App->importer->skel_i->LoadSkeleton(bs.par->file.c_str());
		break;
	case RT_Animation:
		animation.ptr = App->importer->anim_i->LoadAnimation(bs.par->file.c_str());
		break;
	default:
		break;
	}
}

void Resource::UnloadFromMem()
{
	switch (bs.par->type)
	{
	case RT_Prefab:
		break;
	case RT_Mesh:
		mesh.ptr->UnloadMem();
		delete mesh.ptr;
		mesh.ptr = nullptr;
		break;
	case RT_Material:
		mat.ptr->UnloadMem();
		delete mat.ptr;
		mat.ptr = nullptr;
		break;
	case RT_Texture:
		texture.ptr->UnloadMem();
		delete texture.ptr;
		texture.ptr = nullptr;
		break;
	case RT_Billboard:
		/*billboard.ptr->UnloadMem();
		delete billboard.ptr;
		billboard.ptr = nullptr;*/
		break;
	case RT_Skeleton:
		skeleton.ptr->UnloadMem();
		delete skeleton.ptr;
		skeleton.ptr = nullptr;
		break;
	case RT_Animation:
		animation.ptr->UnloadMem();
		delete animation.ptr;
		animation.ptr = nullptr;
		break;
	default:
		break;
	}
}

bool Resource::IsLoaded()
{
	bool ret = false;
	switch(bs.par->type)
	{
	case RT_Prefab:
		break;
	case RT_Mesh:
		ret = (mesh.ptr != nullptr);
		break;
	case RT_Material:
		ret = (mat.ptr != nullptr);
		break;
	case RT_Texture:
		ret = (texture.ptr != nullptr);
		break;
	case RT_Billboard:
		//ret = (billboard.ptr != nullptr);
		break;
	case RT_Skeleton:
		ret = (skeleton.ptr != nullptr);
		break;
	case RT_Animation:
		ret = (animation.ptr != nullptr);
		break;
	default:
		ret = false;
	}

	return ret;
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

bool DGUID::operator==(DGUID cmp_id)
{
	std::vector<uint> f = TrueSum();
	std::vector<uint> s = cmp_id.TrueSum();

	for (int i = 0; i < f.size(); i++)
	{
		if (f[i] != s[i])
			return false;
	}

	return true;
}

bool DGUID::operator==(const DGUID cmp_id)const
{
	bool ret = false;
	for (int i = 0; i < 32; i++)
	{
		ret = MD5ID[i] == cmp_id.MD5ID[i];
		if (!ret)
			break;
	}
	if (ret)
		return !ret;
	else
	{
		return TrueComp(cmp_id);
	}

}

void DGUID::SetInvalid()
{
	for (int i = 0; i < 32; i++)
		MD5ID[i] = -52;
}

bool DGUID::CheckValidity()
{
	bool ret = true;

	for (int i = 0; i < 32 && ret; i++)
	{
		ret = MD5ID[i] != -52;
	}

	return ret;
}

void DGUID::cpyfromstring(std::string cmp_id)
{
	memcpy(MD5ID, cmp_id.c_str(), 32);
}