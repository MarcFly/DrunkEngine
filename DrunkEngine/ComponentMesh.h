#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "Assimp/include/scene.h"
#include "Component.h"

class GameObject;
class ComponentCamera;
struct ResourceMesh;
class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* par) {
		SetBaseVals();

		parent = par;
	};
	//ComponentMesh(const aiMesh* mesh, GameObject* par);

	~ComponentMesh() {};

	bool SetTexCoords(const aiMesh* mesh);
	void SetNormals(const int& index);
	void SetMeshBoundBox();
	
	void Draw();
	void DrawMesh();
	void DrawMeshWire();
	void DrawNormals();

	void CleanUp();

	void Load(JSON_Object* comp);
	void Save(JSON_Array* comps);

public:
	ResourceMesh* r_mesh;
	DGUID Material_Ind;
	uint mat_ind;
	AABB* BoundingBox;

public:
	void SetTextTo(const DGUID& nUID);
	void SetTextTo(ResourceMesh* nMesh);

	void SetBaseVals()
	{
		type = CT_Mesh;
		multiple = true;

		BoundingBox = nullptr;

		multiple = false;

		to_pop = false;

		mat_ind = -1;
	}
};

#endif