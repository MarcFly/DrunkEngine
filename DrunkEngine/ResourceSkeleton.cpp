#include "ResourceSkeleton.h"
#include "Application.h"

Bone::Bone(const Bone* cpy)
{
	this->active = cpy->active;
	this->ID = cpy->ID;
	this->fast_id = cpy->fast_id;
	this->fast_par_id = cpy->fast_par_id;
	this->name = cpy->name;
	this->affected_mesh = cpy->affected_mesh;
	for (int i = 0; i < cpy->weights.size(); ++i)
		this->weights.push_back(new BoneWeight(cpy->weights[i]));

	this->transform = cpy->transform;
}

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
	for (int i = 0; i < AnimSkel.size(); ++i)
	{
		delete AnimSkel[i];
		AnimSkel[i] = nullptr;
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

	CreateAnimSkel(bones);
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

void ResourceSkeleton::CreateAnimSkel(std::vector<Bone*>& bones_to_cpy, Bone* AnimBone)
{
	if(AnimBone == nullptr)
		for (int i = 0; i < bones_to_cpy.size(); ++i)
		{
			AnimSkel.push_back(new Bone(bones_to_cpy[i]));
			CreateAnimSkel(bones_to_cpy[i]->children, AnimSkel[i]);
		}
	else
		for (int i = 0; i < bones_to_cpy.size(); ++i)
		{
			AnimBone->children.push_back(new Bone(bones_to_cpy[i]));
			CreateAnimSkel(bones_to_cpy[i]->children, AnimBone->children[i]);
		}


}

//------------------------------------------------------------------------------------

void MetaSkeleton::LoadMetaFile(const char* file)
{
	App->importer->skel_i->LoadMeta(file, this);
}