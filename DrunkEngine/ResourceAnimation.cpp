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
std::vector<float4x4> AnimChannel::CurrMatrix(float time, float duration, float tickrate)
{
	std::vector<float4x4> ret;

	float3 Translate, TB;
	Quat Rotate, RB;
	float3 Scale, SB;
	for (int i = 0; i < num_translation_keys; i++)
	{
		if (time <= TranslationKeys[i].time || i == num_translation_keys - 1)
		{
			Translate = TranslationKeys[i].value;
			if (i == num_translation_keys - 1)
				TB = TranslationKeys[0].value;
			else
				TB = TranslationKeys[i + 1].value;
			break;
		}
	}
	for (int i = 0; i < num_rotation_keys; i++)
	{
		if (time <= RotationKeys[i].time || i == num_rotation_keys - 1)
		{
			Rotate = RotationKeys[i].value;
			if (i == num_rotation_keys - 1)
				RB = RotationKeys[0].value;
			else
				RB = RotationKeys[i + 1].value;
			break;
		}
	}
	for (int i = 0; i < num_scaling_keys; i++)
	{
		if (time <= ScalingKeys[i].time || i == num_scaling_keys - 1)
		{
			Scale = ScalingKeys[i].value;
			if (i == num_scaling_keys - 1)
				SB = ScalingKeys[0].value;
			else
				SB = ScalingKeys[i + 1].value;
			break;
		}
	}
	ret.push_back(float4x4::FromTRS(Translate, Rotate, Scale));
	ret.push_back(float4x4::FromTRS(TB, RB, SB));

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