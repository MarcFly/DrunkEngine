#ifndef _Res_Mat_
#define _Res_Mat_

#include "Color.h"

struct ResourceTexture;

struct ResourceMaterial
{
	uint NumDiffTextures = 0;
	std::vector<ResourceTexture*> textures;
	uint NumProperties = 0;
	Color default_print = { 1,1,1,1 };
};

#endif