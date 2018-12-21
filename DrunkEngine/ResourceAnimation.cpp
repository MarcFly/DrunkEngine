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
		if (num_translation_keys == 1 || time < TranslationKeys[0].time)
			return &TranslationKeys[num_translation_keys - 1];

		if (i == num_translation_keys - 1 || (time >= TranslationKeys[i].time && time < TranslationKeys[i + 1].time))
			return &TranslationKeys[i];
	}
}

QuatKey* AnimChannel::LastRKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_rotation_keys; ++i)
	{
		if (num_rotation_keys == 1 || time < RotationKeys[0].time)
			return &RotationKeys[num_rotation_keys - 1];

		if (i == num_rotation_keys - 1 || (time >= RotationKeys[i].time && time < RotationKeys[i + 1].time))
			return &RotationKeys[i];
	}
}

float3Key* AnimChannel::LastSKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_scaling_keys; ++i)
	{
		if (num_rotation_keys == 1 || time < ScalingKeys[i].time)
			return &ScalingKeys[num_scaling_keys - 1];

		if (i == num_scaling_keys - 1 || (time >= ScalingKeys[i].time && time < ScalingKeys[i + 1].time))
			return &ScalingKeys[i];
	}
}

float3Key* AnimChannel::NextTKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_translation_keys; ++i)
	{
		if (time < TranslationKeys[i].time || num_translation_keys == 1)
			return &TranslationKeys[i];
	}
	return &TranslationKeys[0];
}

QuatKey* AnimChannel::NextRKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_rotation_keys; ++i)
	{
		if (time < RotationKeys[i].time || num_rotation_keys == 1)
			return &RotationKeys[i];
	}
	return &RotationKeys[0];
}

float3Key* AnimChannel::NextSKey(float time, float duration, float tickrate)
{
	for (int i = 0; i < num_scaling_keys; ++i)
	{
		if (time < ScalingKeys[i].time || num_scaling_keys == 1)
			return &ScalingKeys[i];
	}
	return &ScalingKeys[0];
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

	float3 trans;
	Quat rot;
	float3 scale;

	//Pos
	if (CurrTrans == NextTrans)
		trans = CurrTrans->value;
	else
	{
		float time_between_keys = GetInBetweenKeysTime(time, CurrTrans->time, NextTrans->time, duration);

		trans = NextTrans->value * time_between_keys;
		trans += CurrTrans->value * (1 - time_between_keys);
	}

	//Rot
	if (CurrRot == NextRot)
		rot = CurrRot->value;
	else
	{
		rot = CurrRot->value;

		float time_between_keys = GetInBetweenKeysTime(time, CurrRot->time, NextRot->time, duration);
		float3 rot_test = RadToDeg(NextRot->value.ToEulerXYZ());
		float3 ret_rot;
		ret_rot = rot_test * time_between_keys;
		ret_rot += rot_test * (1 - time_between_keys);
		
		rot = Quat::FromEulerXYZ(DegToRad(ret_rot.x), DegToRad(ret_rot.y), DegToRad(ret_rot.z));
	}

	//Scale
	if (CurrScale == NextScale)
		scale = CurrScale->value;
	else
	{
		float time_between_keys = GetInBetweenKeysTime(time, CurrScale->time, NextScale->time, duration);

		scale = NextScale->value * time_between_keys;
		scale += CurrScale->value * (1 - time_between_keys);
	}

	ret = float4x4::FromTRS(trans, rot, scale);
	
	return ret;
}
float4x4 AnimChannel::GetFirstFrame(float duration, float tickrate)
{
	float3Key* CurrTrans = LastTKey(0, duration, tickrate);

	QuatKey* CurrRot = LastRKey(0, duration, tickrate);

	float3Key* CurrScale = LastSKey(0, duration, tickrate);

	return float4x4::FromTRS(CurrTrans->value, CurrRot->value,CurrScale->value);
}
float AnimChannel::GetInBetweenKeysTime(float curr_time, float key1_time, float key2_time, float total_time) const
{
	if (key1_time < key2_time)
		return (curr_time - key1_time) / (key2_time - key1_time);
	else
		return (curr_time - key1_time) / (key2_time - key1_time + total_time);
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