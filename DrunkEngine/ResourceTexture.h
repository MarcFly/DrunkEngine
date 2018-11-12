#ifndef _Res_Tex_
#define _Res_Tex_

#include <string>
#include "GLEW/include/GL/glew.h"
#include "Resource.h"
#include "MaterialImport.h"

enum TextureMode
{
	TM_Error = -1,
	TM_DIFFUSE,
	TM_AMBIENT,

	TM_MAX
};

struct ResourceTexture
{
	GLuint id_tex = 0;
	GLuint width, height;
	std::string filename;
	TextureMode type = TM_Error;

	bool deleted = false;
};

class MetaTexture : public MetaResource
{
public:
	MetaTexture() {};
	~MetaTexture() {};

	void LoadMetaFile(const char* file)
	{
		App->importer->mat_i->LoadMetaTex(file, this);
	}
};

#endif