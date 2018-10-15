#ifndef _COMPONENT_MATERIAL_
#define _COMPONENT_MATERIAL_

#include "GameObject.h"

enum TextureMode
{
	TM_Error = -1,
	TM_DIFFUSE,
	
	TM_MAX
};

struct Texture
{
	GLuint id_tex = 0;
	GLuint width, height;
	std::string filename;
	TextureMode type = TM_Error;
};

class ComponentMaterial
{
public:
	ComponentMaterial() {};
	ComponentMaterial(aiMaterial* mat, GameObject* par);

	~ComponentMaterial() {};

	void LoadTexture(const char* path, Texture* tex);
	void DestroyTexture();

public:



	uint NumDiffTextures = 0;
	std::list<Texture*> textures;
	uint NumProperties = 0;
	Color default_print = {1,1,1,1};
	

	GameObject* parent = nullptr;
};

#endif