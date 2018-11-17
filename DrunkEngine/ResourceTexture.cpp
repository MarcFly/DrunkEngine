#include "ResourceTexture.h"

ResourceTexture::~ResourceTexture()
{
	UnloadMem();
}

void ResourceTexture::UnloadMem()
{
	if (id_tex != -1)
	{
		glDeleteTextures(1, &id_tex);
		id_tex = -1;
	}
	filename.clear();
}