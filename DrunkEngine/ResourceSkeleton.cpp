#include "ResourceSkeleton.h"
#include "Application.h"

void Bone::OrderBones()
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i]->fast_par_id != 0 && (children[i]->parent == nullptr || children[i]->fast_par_id != children[i]->parent->fast_id))
		{
			Bone* bn = children[i];
			AdjustChildren(i);
			Bone* get = GetChild(bn->fast_par_id);
			bn->parent = get;
			if (get != nullptr)
				get->children.push_back(bn);
			i--;
		}
	}
}

void Bone::AdjustChildren(const int& i)
{
	for (int j = i; j < children.size() - 1; j++)
	{
		children[j] = children[j + 1];
	}

	children.pop_back();
}

Bone* Bone::GetChild(const uint & par_id)
{
	Bone* ret = nullptr;

	for (int i = 0; i < children.size(); i++)
	{
		if (children[i]->fast_id == par_id)
			ret = children[i];
		else
			ret = children[i]->GetChild(par_id);

		if (ret != nullptr)
			break;
	}

	return ret;

}

void Bone::CalculateBoneGlobalTransforms()
{
	if (parent != nullptr)
	{
		transform.global_transform = transform.world_pos * transform.world_rot * parent->transform.global_transform * transform.local_transform;

		//transform.global_transform[0][0] = 1;
		//transform.global_transform[1][1] = 1;
		//transform.global_transform[2][2] = 1;

		transform.global_pos = transform.global_transform.Col3(3);
		transform.global_rot = transform.GetRotFromMat(transform.global_transform);
		transform.global_scale.x = transform.global_transform[0][0];
		transform.global_scale.y = transform.global_transform[1][1];
		transform.global_scale.z = transform.global_transform[2][2];
	}

	for (int i = 0; i < children.size(); i++)
		children[i]->CalculateBoneGlobalTransforms();
}

Bone * Bone::FindBone(const std::string & bonename)
{
	Bone* ret = nullptr;

	for (int i = 0; i < children.size() && ret == nullptr; i++)
	{
		if (bonename.find(children[i]->name.c_str()) != std::string::npos)
			ret = children[i];
		if (ret == nullptr)
			ret = children[i]->FindBone(bonename);
	}

	return ret;
}

//------------------------------------------------------------------------------------

ResourceSkeleton::~ResourceSkeleton()
{
	UnloadMem();
}



void ResourceSkeleton::UnloadMem()
{
	for (int i = 0; i < bones.size(); i++)
	{
		delete bones[i];
		bones[i] = nullptr;
	}
	bones.clear();
}

void ResourceSkeleton::OrderBones()
{
	for (int i = 0; i < bones.size(); i++)
	{
		if (bones[i]->fast_par_id != 0 && (bones[i]->parent == nullptr || bones[i]->fast_par_id != bones[i]->parent->fast_id))
		{
			Bone* bn = bones[i];
			AdjustChildren(i);
			Bone* get = GetChild(bn->fast_par_id);
			bn->parent = get;
			if (get != nullptr)
				get->children.push_back(bn);
			i--;
		}
	}
}

void ResourceSkeleton::AdjustChildren(const int& i)
{
	for (int j = i; j < bones.size() - 1; j++)
	{
		bones[j] = bones[j + 1];
	}

	bones.pop_back();
}

Bone* ResourceSkeleton::GetChild(const uint & par_id)
{
	Bone* ret = nullptr;

	for (int i = 0; i < bones.size(); i++)
	{
		if (bones[i]->fast_id == par_id)
			ret = bones[i];
		else
			ret = bones[i]->GetChild(par_id);

		if (ret != nullptr)
			break;
	}

	return ret;

}

Bone * ResourceSkeleton::FindBone(const std::string & bonename)
{
	Bone* ret = nullptr;

	for (int i = 0; i < bones.size() && ret == nullptr; i++)
	{
		if (bonename.find(bones[i]->name) != std::string::npos)
			ret = bones[i];
		if (ret == nullptr)
			ret = bones[i]->FindBone(bonename);
	}

	return ret;
}

void ResourceSkeleton::CalculateSkeletonTransforms()
{
	for (int i = 0; i < bones.size(); i++)
		bones[i]->CalculateBoneGlobalTransforms();
}

//------------------------------------------------------------------------------------

void MetaSkeleton::LoadMetaFile(const char* file)
{
	App->importer->skel_i->LoadMeta(file, this);
}