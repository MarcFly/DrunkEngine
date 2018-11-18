#ifndef _COMPONENT_MATERIAL_
#define _COMPONENT_MATERIAL_

#include <vector>
#include <string>
#include "Color.h"
#include "Component.h"
#include "Globals.h"

class GameObject;
struct ResourceTexture;
struct ResourceMaterial;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial() { SetBaseVals(); };
	ComponentMaterial(GameObject* par);

	~ComponentMaterial() {};

	void PopTexture(const int& tex_index);

	void CleanUp();


	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

public:

	ResourceMaterial* r_mat = nullptr;
	std::vector<ResourceTexture*> textures;

public:
	void SetBaseVals()
	{
		this->type = CT_Material;

		this->multiple = true;

		this->parent = nullptr;

		this->to_pop = false;
	}
};

#endif