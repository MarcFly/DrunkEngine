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

class GameObject
{
public:
	GameObject();
	GameObject(const char* path, const aiScene* scene, const aiNode* root_obj, const char* file_path);

	void CreateThisObj(const aiScene* scene, const aiNode* obj);

	void Start();
	void Update(float dt);
	void Draw();
	void DrawBB();

	void Save(JSON_Object* root_value);
	void Load(JSON_Object* root_value);

	vec getObjectCenter();
	float SetBoundBox();
	void SetBoundBoxFromMeshes();
	//void SetBoundBody();

	void AdjustObjects();
	void AdjustComponents();

	void CleanUp();

	std::vector<unsigned int> GetMeshProps();


public:
	Uint32	UUID, par_UUID = UINT_LEAST32_MAX;
	std::string name;

	AABB* BoundingBox = nullptr;

	GameObject* parent = nullptr;
	GameObject* root = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	bool to_pop = false;
	bool active = false;

public:
	ComponentCamera * camera = nullptr;
	std::vector<ComponentCamera*> cameras;

public:

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


};

#endif
