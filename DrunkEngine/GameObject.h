#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include <vector>
#include <string>
#include "Assimp/include/scene.h"
#include "MGL/MathGeoLib.h"

class ComponentMesh;
class ComponentMaterial;
class ComponentTransform;
class Primitive;

class GameObject
{
public:
	GameObject() {};
	GameObject(const aiScene* scene, const aiNode* obj_root, GameObject* par);
	GameObject(const aiScene* scene, const aiNode* root_obj, const char* file_path);

	void CreateThisObj(const aiScene* scene, const aiNode* obj);

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
	bool selected = 0;

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
