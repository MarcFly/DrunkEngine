#include "ResourceSkeleton.h"
#include "Application.h"

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

//------------------------------------------------------------------------------------

void MetaSkeleton::LoadMetaFile(const char* file)
{
	App->importer->skel_i->LoadMeta(file, this);
}