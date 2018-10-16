#ifndef _COMPONENT_MATERIAL_
#define _COMPONENT_MATERIAL_

#include <vector>
#include <string>
#include "GLEW/include/GL/glew.h"
#include "Assimp/include/scene.h"
#include "Color.h"

class GameObject;

enum TextureMode
{
	TM_Error = -1,
	TM_DIFFUSE,
	
	TM_MAX
};

class ComponentMaterial;

struct Texture
{
	GLuint id_tex = 0;
	GLuint width, height;
	std::string filename;
	TextureMode type = TM_Error;
	ComponentMaterial* mparent;

	std::vector<ComponentMaterial*> referenced_mats;

	bool deleted = false;
};

class ComponentMaterial
{
public:
	ComponentMaterial() {};
	ComponentMaterial(aiMaterial* mat, GameObject* par);

	~ComponentMaterial() {};

	void LoadTexture(const char* path);
	void DestroyTexture(const int& tex_index);

	void CleanUp();

	Texture* CheckTexRep(std::string name);
	Texture* CheckNameRep(std::string name);

public:

	unsigned int NumDiffTextures = 0;
	std::vector<Texture*> textures;
	unsigned int NumProperties = 0;
	Color default_print = {1,1,1,1};

	GameObject* parent = nullptr;

	bool to_pop = false;
};

#endif