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
	for (int i = 0; i < children.size(); i++)
	{
		ComponentTransform* b_child = &children[i]->transform;
		b_child->global_transform = b_child->world_pos * b_child->world_rot * children[i]->parent->transform.global_transform * b_child->local_transform;

		b_child->global_pos = b_child->global_transform.Col3(3);
		b_child->global_rot = b_child->GetRotFromMat(b_child->global_transform);
		b_child->global_scale = b_child->global_transform.GetScale();

		children[i]->CalculateBoneGlobalTransforms();
	}
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