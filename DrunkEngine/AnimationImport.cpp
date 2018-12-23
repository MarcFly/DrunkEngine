#include "AnimationImport.h"
#include "ResourceAnimation.h"
#include "ComponentAnimation.h"
#include "Application.h"
#include "SkeletonImport.h"

#include <fstream>
#include <iostream>

void AnimationImport::Init()
{

}

AnimToExport AnimationImport::PrepSkeletonAnimationExport(std::multimap<uint, BoneCrumb*>& Skeleton, aiAnimation* anim)
{
	AnimToExport ret;
	ret.LinkedAnim = anim;

	for (int i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* curr = anim->mChannels[i];
		std::string curr_channel_name = curr->mNodeName.C_Str();
		for (std::multimap<uint, BoneCrumb*>::iterator it = Skeleton.begin(); it != Skeleton.end(); it++)
		{
			std::string cmp_name = it->second->BoneNode->mName.C_Str();
			if (curr_channel_name.find(cmp_name) != std::string::npos)
			{
				ret.AnimNodes.push_back(curr);
				break;
			}
		}
		
	}

	return ret;
}

void AnimationImport::ExportSkelAnimation(std::multimap<uint, BoneCrumb*>& Skeleton, AnimToExport& anim, const int& anim_id, const char* path)
{
	std::string filename = ".\\Library\\";
	filename += GetFileName(path) + "_Anim_" += std::to_string(anim_id);

	std::fstream write_file;

	write_file.open((filename + ".animdrnk").c_str(), std::fstream::out | std::fstream::binary);

	uint buf_size = 0;
	uint num_channels = anim.AnimNodes.size();

	std::string name = anim.LinkedAnim->mName.C_Str();
	uint name_size = name.length() + 1;

	buf_size = sizeof(uint) + name_size + sizeof(uint);
	char* data = new char[buf_size];
	char* cursor = data;

	memcpy(cursor, &name_size, sizeof(uint));
	cursor += sizeof(uint);
	memcpy(cursor, name.c_str(), name.length());
	cursor += name.length();
	memcpy(cursor, "\0", 1);
	cursor += 1;
	memcpy(cursor, &num_channels, sizeof(uint));
	cursor += sizeof(uint);

	write_file.write(data, buf_size);
	delete[] data;
	data = nullptr;

	for (int i = 0; i < anim.AnimNodes.size(); i++)
	{
		write_file.close();
		write_file.open((filename + ".animdrnk").c_str(), std::fstream::app | std::fstream::binary);

		aiNodeAnim* curr = anim.AnimNodes[i];
		buf_size = 0;
		name = curr->mNodeName.C_Str();
		name_size = name.length() + 1;
		buf_size += sizeof(uint) + name_size + sizeof(uint) * 3;
		buf_size += (sizeof(double) + sizeof(float3)) * curr->mNumPositionKeys;
		buf_size += (sizeof(double) + sizeof(aiQuaternion)) * curr->mNumRotationKeys;
		buf_size += (sizeof(double) + sizeof(float3)) * curr->mNumScalingKeys;
		data = new char[buf_size];
		cursor = data;

		memcpy(cursor, &name_size, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, name.c_str(), name.length());
		cursor += name.length();
		memcpy(cursor, "\0", 1);
		cursor += 1;

		memcpy(cursor, &curr->mNumPositionKeys, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, &curr->mNumRotationKeys, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, &curr->mNumScalingKeys, sizeof(uint));
		cursor += sizeof(uint);

		for (int j = 0; j < curr->mNumPositionKeys; j++)
		{
			memcpy(cursor, &curr->mPositionKeys[j].mTime, sizeof(double));
			cursor += sizeof(double);
			memcpy(cursor, &curr->mPositionKeys[j].mValue, sizeof(aiVector3D));
			cursor += sizeof(aiVector3D);
		}
		//memcpy(cursor, &curr->mPositionKeys, (sizeof(float3Key)) * curr->mNumPositionKeys);
		//cursor += (sizeof(float3Key)) * curr->mNumPositionKeys;

		for (int j = 0; j < curr->mNumRotationKeys; j++)
		{
			memcpy(cursor, &curr->mRotationKeys[j].mTime, sizeof(double));
			cursor += sizeof(double);
			memcpy(cursor, &curr->mRotationKeys[j].mValue, sizeof(aiQuaternion));
			cursor += sizeof(aiQuaternion);
		}
		//memcpy(cursor, &curr->mRotationKeys, (sizeof(QuatKey)) * curr->mNumRotationKeys);
		//cursor += (sizeof(QuatKey)) * curr->mNumRotationKeys;

		for (int j = 0; j < curr->mNumScalingKeys; j++)
		{
			memcpy(cursor, &curr->mScalingKeys[j].mTime, sizeof(double));
			cursor += sizeof(double);
			memcpy(cursor, &curr->mScalingKeys[j].mValue, sizeof(aiVector3D));
			cursor += sizeof(aiVector3D);
		}

		//memcpy(cursor, &curr->mScalingKeys, (sizeof(float3Key)) * curr->mNumScalingKeys);
		//cursor += (sizeof(float3Key)) * curr->mNumScalingKeys;

		write_file.write(data, buf_size);
		write_file.close();

		delete[] data;
		data = nullptr;
	}

	ExportMeta(anim.LinkedAnim, anim_id, path);
}

void AnimationImport::LinkAnim(DGUID fID, ComponentAnimation* anim)
{
	MetaAnimation* res = (MetaAnimation*)App->resources->Library.at(fID);

	if (res->res_found)
	{
		anim->name = res->file;

		if (!res->Asset.IsLoaded())
			res->Asset.LoadToMem();

		if (anim->anims.size() == 0)
		{
			VirtualAnimation push;
			push.end = res->duration;
			push.tickrate = res->tickrate;
			anim->anims.push_back(push);
		}

		anim->duration = res->duration;

		res->UseCount++;
	}

	anim->r_anim = res->Asset.animation.ptr;
	anim->UID = fID;
}

ResourceAnimation* AnimationImport::LoadAnimation(const char* file)
{
	ResourceAnimation* ret = new ResourceAnimation();

	std::ifstream read_file;
	read_file.open(file, std::fstream::in | std::fstream::binary);

	int size = read_file.seekg(0, read_file.end).tellg();
	read_file.seekg(0, read_file.beg);

	if (size > 0)
	{
		char* data = new char[size];
		read_file.read(data, sizeof(char)*size);
		char* cursor = data;

		uint name_size;
		memcpy(&name_size, cursor, sizeof(uint));
		cursor += sizeof(uint);

		char* cpy_name = new char[name_size];
		memcpy(cpy_name, cursor, name_size);
		cursor += name_size;
		ret->name = std::string(cpy_name, name_size);
		delete[] cpy_name;
		
		uint num_channels;
		memcpy(&num_channels, cursor, sizeof(uint));
		cursor += sizeof(uint);

		for (int i = 0; i < num_channels; i++)
		{
			AnimChannel* curr = new AnimChannel();

			name_size = 0;
			memcpy(&name_size, cursor, sizeof(uint));
			cursor += sizeof(uint);

			cpy_name = new char[name_size];
			memcpy(cpy_name, cursor, name_size);
			cursor += name_size;
			curr->bone_name = std::string(cpy_name, name_size);
			delete[] cpy_name;

			memcpy(&curr->num_translation_keys, cursor, sizeof(uint));
			cursor += sizeof(uint);
			memcpy(&curr->num_rotation_keys, cursor, sizeof(uint));
			cursor += sizeof(uint);
			memcpy(&curr->num_scaling_keys, cursor, sizeof(uint));
			cursor += sizeof(uint);

			curr->TranslationKeys = new float3Key[curr->num_translation_keys];
			for (int j = 0; j < curr->num_translation_keys; j++)
			{
				memcpy(&curr->TranslationKeys[j].time, cursor, sizeof(double));
				cursor += sizeof(double);
				memcpy(&curr->TranslationKeys[j].value, cursor, sizeof(float3));
				cursor += sizeof(float3);
			}

			curr->RotationKeys = new QuatKey[curr->num_rotation_keys];
			for (int j = 0; j < curr->num_rotation_keys; j++)
			{
				memcpy(&curr->RotationKeys[j].time, cursor, sizeof(double));
				cursor += sizeof(double);
				memcpy(&curr->RotationKeys[j].value, cursor, sizeof(Quat));
				float intermediate = curr->RotationKeys[j].value.w;
				curr->RotationKeys[j].value.w = curr->RotationKeys[j].value.x;
				curr->RotationKeys[j].value.x = intermediate;
				cursor += sizeof(Quat);
			}

			curr->ScalingKeys = new float3Key[curr->num_scaling_keys];
			for (int j = 0; j < curr->num_scaling_keys; j++)
			{
				memcpy(&curr->ScalingKeys[j].time, cursor, sizeof(double));
				cursor += sizeof(double);
				memcpy(&curr->ScalingKeys[j].value, cursor, sizeof(float3));
				cursor += sizeof(float3);
			}

			ret->channels.push_back(curr);
		}
	}
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

void AnimationImport::ExportMeta(const aiAnimation* anim, const int& anim_id, std::string path)
{
	std::string meta_name = ".\\Library\\" + GetFileName(path.c_str()) + "_Anim_" + std::to_string(anim_id) + ".meta";
	JSON_Value* meta_file = json_parse_file(path.c_str());
	meta_file = json_value_init_object();

	JSON_Object* meta_obj = json_value_get_object(meta_file);
	std::string write = meta_name.substr(0, meta_name.length() - 5) + ".animdrnk";

	json_object_dotset_string(meta_obj, "File", write.c_str());
	
	json_object_dotset_number(meta_obj, "duration", anim->mDuration);
	json_object_dotset_number(meta_obj, "tickrate", anim->mTicksPerSecond);

	json_serialize_to_file(meta_file, meta_name.c_str());

	// Free Meta Value
	json_object_clear(meta_obj);
	json_value_free(meta_file);
}

void AnimationImport::LoadMeta(const char* file, MetaAnimation* meta)
{
	meta->type = RT_Animation;

	JSON_Value* meta_file = json_parse_file(file);
	if (meta_file != nullptr)
	{
		meta->res_found = true;
		JSON_Object* meta_obj = json_value_get_object(meta_file);

		meta->file = json_object_dotget_string(meta_obj, "File");
		meta->duration = json_object_dotget_number(meta_obj, "duration");
		meta->tickrate = json_object_dotget_number(meta_obj, "tickrate");

		// Free Meta Value
		json_object_clear(meta_obj);
		json_value_free(meta_file);
	}
}

//------------------------------

ComponentAnimation* AnimationImport::ImportAnimationForSkeleton(ResourceSkeleton* skel, GameObject* par)
{
	/*OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "DrunkEngine Scenes (*.drnk*)\0*.drnk*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";

	GetSaveFileName(&ofn);
	if (fileName[0] != '\0')
	{
		const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_Fast);
	}*/

	return nullptr;
}