#include "AnimationImport.h"
#include "ResourceAnimation.h"
#include "ComponentAnimation.h"
#include "Application.h"

#include <fstream>
#include <iostream>

void AnimationImport::Init()
{

}

void AnimationImport::LinkAnim(DGUID fID, ComponentAnimation* anim)
{
	MetaAnimation* res = (MetaAnimation*)App->resources->Library.at(fID);

	anim->name = res->file;

	if (!res->Asset.IsLoaded())
		res->Asset.LoadToMem();

	anim->r_anim = res->Asset.animation.ptr;
	anim->UID = fID;
	anim->duration = res->duration;
	anim->tickrate = res->tickrate;

	res->UseCount++;
}

void AnimationImport::ExportAIAnimation(const aiAnimation* anim, const int& anim_id, const char* path)
{
	std::string filename = ".\\Library\\";
	filename += GetFileName(path) + "_Anim_" + std::to_string(anim_id);

	std::fstream write_file;

	write_file.open((filename + ".animdrnk").c_str(), std::fstream::out | std::fstream::binary);

	uint buf_size = 0;

	uint num_channels = anim->mNumChannels;

	std::string name = anim->mName.C_Str();
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

	write_file.close();
	write_file.open((filename + ".animdrnk").c_str(), std::fstream::app | std::fstream::binary);

	for (int i = 0; i < num_channels; i++)
	{
		aiNodeAnim* curr = anim->mChannels[i];
		buf_size = 0;
		name = curr->mNodeName.C_Str();
		name_size = name.length() + 1;
		buf_size += sizeof(uint) + name_size + sizeof(uint) * 3;
		buf_size += (sizeof(double) + sizeof(float) * 3) * curr->mNumPositionKeys;
		buf_size += (sizeof(double) + sizeof(float) * 3) * curr->mNumScalingKeys;
		buf_size += (sizeof(double) + sizeof(Quat)) * curr->mNumRotationKeys;
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

		memcpy(cursor, &curr->mPositionKeys, (sizeof(double) + sizeof(float) * 3) * curr->mNumPositionKeys);
		cursor += (sizeof(double) + sizeof(float) * 3) * curr->mNumPositionKeys;

		memcpy(cursor, &curr->mRotationKeys, (sizeof(double) + sizeof(Quat)) * curr->mNumRotationKeys);
		cursor += (sizeof(double) + sizeof(Quat)) * curr->mNumRotationKeys;

		memcpy(cursor, &curr->mScalingKeys, (sizeof(double) + sizeof(float) * 3) * curr->mNumScalingKeys);
		cursor += (sizeof(double) + sizeof(float) * 3) * curr->mNumScalingKeys;
	
		write_file.write(data, buf_size);

		delete[] data;
		data = nullptr;
	}

	write_file.close();

	ExportMeta(anim, anim_id, path);
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
			memcpy(curr->TranslationKeys, cursor, sizeof(float3Key) * curr->num_translation_keys);
			cursor += sizeof(float3Key) * curr->num_translation_keys;

			curr->RotationKeys = new QuatKey[curr->num_rotation_keys];
			memcpy(curr->RotationKeys, cursor, sizeof(QuatKey) * curr->num_rotation_keys);
			cursor += sizeof(QuatKey) * curr->num_rotation_keys;

			curr->ScalingKeys = new float3Key[curr->num_scaling_keys];
			memcpy(curr->ScalingKeys, cursor, sizeof(float3Key) * curr->num_scaling_keys);
			cursor += sizeof(float3Key) * curr->num_scaling_keys;

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
	JSON_Object* meta_obj = json_value_get_object(meta_file);

	meta->file = json_object_dotget_string(meta_obj, "File");
	meta->duration = json_object_dotget_number(meta_obj, "duration");
	meta->tickrate = json_object_dotget_number(meta_obj, "tickrate");

	// Free Meta Value
	json_object_clear(meta_obj);
	json_value_free(meta_file);
}