#ifndef _Res_Tex_
#define _Res_Tex_

#include <string>
#include "GLEW/include/GL/glew.h"
#include "Resource.h"
#include "MaterialImport.h"
#include "Application.h"

enum TextureMode
{
	TM_Error = -1,
	TM_DIFFUSE,
	TM_SPECULAR,
	TM_AMBIENT,
	TM_EMISSIVE,
	TM_HEIGHT,
	TM_NORMALS,
	TM_SHININESS,
	TM_OPACITY,
	TM_DISPLACEMENT,
	TM_LIGTHMAP,
	TM_REFLECTION,

	TM_MAX
};

struct ResourceTexture
{
	GLuint id_tex = 0;
	GLuint width, height;
	std::string filename;
	TextureMode type = TM_Error;

	~ResourceTexture();

	void UnloadMem();
};

class MetaTexture : public MetaResource
{
public:
	MetaTexture() { type = RT_Texture; };
	~MetaTexture() {};

	void LoadMetaFile(const char* file)
	{
		App->importer->mat_i->LoadMetaTex(file, this);
	}

	TextureMode tex_type = TM_Error;
};

#endif