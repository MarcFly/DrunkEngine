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