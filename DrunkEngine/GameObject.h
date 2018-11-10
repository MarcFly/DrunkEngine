#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include <vector>
#include <string>
#include "Assimp/include/scene.h"
#include "MGL/MathGeoLib.h"
#include "pcg-c-basic-0.9\pcg_basic.h"
#include <Windows.h>
#include "SDL/include/SDL_stdinc.h"
#include <math.h>
#include "parson/parson.h"
#include "ComponentTransform.h"
#include <memory>

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
	GameObject(GameObject* par, const char* name, CTypes type);
	GameObject(const char* path, const aiScene* scene, const aiNode* root_obj, const char* file_path, GameObject* par = nullptr);

	void CreateThisObj(const aiScene* scene, const aiNode* obj);

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
	void AdjustObjects();
	void AdjustComponents();

	bool isInsideFrustum(const ComponentCamera * cam, const AABB* bounding_box);
	void CalculateGlobalTransforms();

	void RecursiveSetStatic(GameObject* obj, const bool bool_static);

	void CleanUp();

	std::vector<unsigned int> GetMeshProps();


public:
	Uint32	UUID = UINT_FAST32_MAX, par_UUID = UINT_FAST32_MAX;
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

	void SetUUID()
	{
		pcg32_random_t rng;
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		QueryPerformanceCounter(&li);
		pcg32_srandom_r(&rng, li.QuadPart, (intptr_t)&rng);

		UUID = pcg32_random_r(&rng);
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
