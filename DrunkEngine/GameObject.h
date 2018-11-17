#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include <vector>
#include <string>
#include "Assimp/include/scene.h"
#include "MGL/MathGeoLib.h"
#include <Windows.h>
#include <math.h>
#include "parson/parson.h"
#include "ComponentTransform.h"
#include <memory>
#include "FileHelpers.h"
#include "ModuleResourceManager.h"

class Component;
class ComponentMesh;
class ComponentMaterial;
class ComponentCamera;
class Primitive;
class KDTree;

class GameObject
{
public:
	GameObject();
	GameObject(const char* name, GameObject* par = nullptr);

	void Start();
	void Update(float dt);
	void Draw();
	void DrawBB();

	void Save(JSON_Array* go);
	void Load(JSON_Value* scene, const char* file);

	vec getObjectCenter();
	float SetBoundBox();
	void SetTransformedBoundBox();
	void SetBoundBoxFromMeshes();

	Component* NewComponent(CTypes type);
	void OrderChildren();
	void AdjustObjects();
	void AdjustComponents();

	void CalculateGlobalTransforms();

	void RecursiveSetStatic(GameObject* obj, const bool bool_static);

	void CleanUp();

	std::vector<unsigned int> GetMeshProps();


public:
	Uint32	UUID = UINT_FAST32_MAX, par_UUID = UINT_FAST32_MAX;
	DGUID UID;
	std::string name;

	AABB* BoundingBox = nullptr;

	GameObject* parent = nullptr;
	GameObject* root = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	bool to_pop = false;
	bool static_pop = false;
	bool active = false;
	bool is_static = false;

	bool static_to_draw = false;

	float max_distance_point = 0;

public:

	AABB* GetBB()
	{
		if (BoundingBox == nullptr)
			SetBoundBox();

		return BoundingBox;
	}

	void DestroyThisObject() 
	{
		this->CleanUp();
		this->to_pop = true;
		parent->AdjustObjects();
	}

	void DestroyComponent()
	{
		// Destroy a component
	}

	ComponentTransform* GetTransform()
	{
		for (int i = 0; i < components.size(); i++)
		{
			if (components[i]->type == CT_Transform)
				return components[i]->AsTransform();
		}

		components.push_back(new ComponentTransform(this));
		return components[components.size() - 1]->AsTransform();
	}

	Component* GetComponent(CTypes id_t)
	{
		for (int i = 0; i < components.size(); i++)
			if (components[i]->type == id_t)
				return components[i];

		return nullptr;
	}

	unsigned int GetCountCType(CTypes id_t)
	{
		unsigned int ret = 0;
		for (int i = 0; i < components.size(); i++)
			if (components[i]->type == id_t)
				ret++;
		return ret;
	}

	std::vector<Component*> GetComponents(CTypes id_t)
	{
		std::vector<Component*> ret;
		for (int i = 0; i < components.size(); i++)
			if (components[i]->type == id_t)
			{
				ret.push_back(components[i]);
			}

		return ret;
	}

	GameObject* GetChild(Uint32 UUID)
	{
		GameObject* ret = nullptr;

		for (int i = 0; i < children.size(); i++)
		{
			if (children[i]->UUID == UUID)
				ret = children[i];
			else
				ret = children[i]->GetChild(UUID);

			if (ret != nullptr)
				break;
		}

		return ret;
	}
};

#endif
