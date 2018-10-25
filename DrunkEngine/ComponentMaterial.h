#ifndef _COMPONENT_MATERIAL_
#define _COMPONENT_MATERIAL_

#include <vector>
#include <string>
#include "GLEW/include/GL/glew.h"
#include "Color.h"
#include "Component.h"
#include "Globals.h"

class GameObject;
enum TextureMode
{
	TM_Error = -1,
	TM_DIFFUSE,
	TM_AMBIENT,

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

class ComponentMaterial : public Component
{
public:
	ComponentMaterial() { SetBaseVals(); };
	ComponentMaterial(GameObject* par);

	~ComponentMaterial() {};

	void DestroyTexture(const int& tex_index);
	void PopTexture(const int& tex_index);

	void CleanUp();

	Texture* CheckTexRep(std::string name);
	Texture* CheckNameRep(std::string name);

public:

	uint NumDiffTextures;
	std::vector<Texture*> textures;
	uint NumProperties;
	Color default_print;

	GameObject* parent = nullptr;

	bool to_pop = false;

public:
	void SetBaseVals()
	{
		this->type = CT_Material;
		this->multiple = true;

		this->NumDiffTextures = 0;
		this->NumProperties = 0;
		this->default_print = { 1,1,1,1 };

		this->parent = nullptr;

		this->to_pop = false;
	}
};

#endif