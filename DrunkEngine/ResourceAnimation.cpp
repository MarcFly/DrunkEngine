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

float3Key* AnimChannel::LastTKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_translation_keys; ++i)
	{
		if (time <= TranslationKeys[i].time || i == num_translation_keys - 1)
			return &TranslationKeys[i];
	}
}

QuatKey* AnimChannel::LastRKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_rotation_keys; ++i)
	{
		if (time <= RotationKeys[i].time || i == num_rotation_keys - 1)
			return &RotationKeys[i];
	}
}

float3Key* AnimChannel::LastSKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_scaling_keys; ++i)
	{
		if (time <= ScalingKeys[i].time || i == num_scaling_keys - 1)
			return &ScalingKeys[i];
	}
}

float3Key* AnimChannel::NextTKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_translation_keys; ++i)
	{
		if (time <= TranslationKeys[i].time)
		{
			if (i == num_translation_keys - 1)
				return &TranslationKeys[0];
			else
				return &TranslationKeys[i + 1];
		}
	}
}

QuatKey* AnimChannel::NextRKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_rotation_keys; ++i)
	{
		if (time <= RotationKeys[i].time)
		{
			if (i == num_rotation_keys - 1)
				return &RotationKeys[0];
			else
				return &RotationKeys[i + 1];
		}
	}
}

float3Key* AnimChannel::NextSKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_scaling_keys; ++i)
	{
		if (time <= ScalingKeys[i].time)
		{
			if (i == num_scaling_keys - 1)
				return &ScalingKeys[0];
			else
				return &ScalingKeys[i + 1];
		}
	}
}


float4x4 AnimChannel::GetMatrix(float time, float duration, float tickrate)
{
	float4x4 ret;

	float3Key* CurrTrans = LastTKey(time, duration, tickrate);
	float3Key* NextTrans = NextTKey(time, duration, tickrate);
	QuatKey* CurrRot = LastRKey(time, duration, tickrate);
	QuatKey* NextRot = NextRKey(time, duration, tickrate);
	float3Key* CurrScale = LastSKey(time, duration, tickrate);
	float3Key* NextScale = NextSKey(time, duration, tickrate);

	float3 Trans;
	Quat Rot;
	float3 Scale;

	if (CurrTrans == NextTrans)
		Trans = CurrTrans->value;
	else
	{

	}

	if (CurrRot == NextRot)
		Rot = CurrRot->value;
	else
	{

	}

	if (CurrScale == NextScale)
		Scale = CurrScale->value;
	else
	{

	}

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