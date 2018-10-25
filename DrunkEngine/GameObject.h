#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include <vector>
#include <string>
#include "Assimp/include/scene.h"
#include "MGL/MathGeoLib.h"
#include "pcg-c-basic-0.9\pcg_basic.h"
#include <time.h>
#include "SDL/include/SDL_stdinc.h"
#include <math.h>

class ComponentMesh;
class ComponentMaterial;
class ComponentTransform;
class ComponentCamera;
class Primitive;

class GameObject
{
public:
	GameObject() {
		pcg32_random_t rng;
		pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, 5); 
		float value = ldexp(pcg32_random_r(&rng), -32);

		UUID = value * (10^8);
	};
	//GameObject(const aiScene* scene, const aiNode* obj_root, GameObject* par);
	GameObject(const char* path, const aiScene* scene, const aiNode* root_obj, const char* file_path);

	void CreateThisObj(const aiScene* scene, const aiNode* obj);

	void Start();
	void Update(float dt);
	void Draw();
	void DrawBB();

	vec getObjectCenter();
	float SetBoundBox();
	void SetBoundBoxFromMeshes();
	//void SetBoundBody();

	void AdjustObjects();
	void AdjustMaterials();
	void AdjustMeshes();

	void CleanUp();

public:
	Uint32	UUID;
	std::string name;
	ComponentTransform* transform = nullptr;
	std::vector<ComponentMesh*> meshes;
	std::vector<ComponentMaterial*> materials;

	AABB* BoundingBox = nullptr;

	Primitive* BoundingBody = nullptr; // In case we create a premade object // Temporary solution

	GameObject* parent = nullptr;
	GameObject* root = nullptr;
	std::vector<GameObject*> children;

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

	void DestroyMaterial();
	void DestroyMesh();

};

#endif
