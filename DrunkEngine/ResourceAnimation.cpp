#include "ResourceAnimation.h"
#include "Application.h"

void AnimChannel::UnloadMem()
{
	for (int i = 0; i < num_translation_keys; i++)
		delete &TranslationKeys[i];
	num_translation_keys = 0;

	for (int i = 0; i < num_rotation_keys; i++)
		delete &RotationKeys[i];
	num_rotation_keys = 0;

	for (int i = 0; i < num_scaling_keys; i++)
		delete &ScalingKeys[i];
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