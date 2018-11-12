#ifndef _Res_Tex_
#define _Res_Tex_

#include <string>
#include "GLEW/include/GL/glew.h"

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

#endif