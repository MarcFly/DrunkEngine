#include "ResourceMaterial.h"
#include "Application.h"

ResourceMaterial::~ResourceMaterial()
{
	UnloadMem();
}

void ResourceMaterial::UnloadMem()
{
	for (int i = 0; i < textures.size(); i++)
		App->resources->Unused(textures[i]);

	textures.clear();
}