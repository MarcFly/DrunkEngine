#include "ResourceAnimation.h"
#include "Application.h"

void AnimChannel::UnloadMem()
{
	if(TranslationKeys != nullptr)
		delete[] TranslationKeys;
	TranslationKeys = nullptr;
	num_translation_keys = 0;
	if(RotationKeys != nullptr)
		delete[] RotationKeys;
	RotationKeys = nullptr;
	num_rotation_keys = 0;

	if(ScalingKeys != nullptr)
		delete[] ScalingKeys;
	ScalingKeys = nullptr;
	num_scaling_keys = 0;
}
float4x4 AnimChannel::CurrMatrix(float time, float duration, float tickrate)
{
	float4x4 ret;

	float3 Translate;
	Quat Rotate;
	float3 Scale;
	for (int i = 0; i < num_translation_keys; i++)
	{
		if (time <= TranslationKeys[i].time || i == num_translation_keys - 1)
		{
			Translate = TranslationKeys[i].value;
			break;
		}
	}
	for (int i = 0; i < num_rotation_keys; i++)
	{
		if (time <= RotationKeys[i].time || i == num_rotation_keys - 1)
		{
			Rotate = RotationKeys[i].value;
			break;
		}
	}
	for (int i = 0; i < num_scaling_keys; i++)
	{
		if (time <= ScalingKeys[i].time || i == num_scaling_keys - 1)
		{
			Scale = ScalingKeys[i].value;
			break;
		}
	}
	ret.FromTRS(Translate, Rotate, Scale);

	return ret;
}
void ResourceAnimation::UnloadMem()
{
	for (int i = 0; i < channels.size(); i++)
	{
		delete channels[i];
		channels[i] = nullptr;
	}
	channels.clear();
}

void MetaAnimation::LoadMetaFile(const char* file)
{
	App->importer->anim_i->LoadMeta(file, this);
}