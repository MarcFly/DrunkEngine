#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include <vector>
#include <string>
#include "Assimp/include/scene.h"
#include "MGL/MathGeoLib.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

class GameObject
{
public:
	GameObject() {};
	GameObject(const aiScene* scene, const aiNode* obj_root, GameObject* par);
	GameObject(const aiScene* scene, const aiNode* root_obj, const char* file_path);

	void CreateThisObj(const aiScene* scene, const aiNode* obj);

	void Draw();

	vec getObjectCenter();
	void SetBoundBox();
	void SetBoundBody();

	void DestroyObject();

	void DestroyMaterial();

public:
	std::string name;
	ComponentTransform* transform = nullptr;
	std::vector<ComponentMesh*> meshes;
	std::vector<ComponentMaterial*> materials;

	AABB BoundingBox;

	Primitive* mBoundingBody = nullptr; // In case we create a premade object // Temporary solution

	GameObject* parent = nullptr;
	GameObject* root = nullptr;
	std::vector<GameObject*> children;
};

#endif
