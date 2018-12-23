#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "Component.h"

class GameObject;
class ComponentCamera;
struct ResourceMesh;
class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* par);

	~ComponentMesh() {};

	void SetNormals(const int& index);
	void SetMeshBoundBox();
	
	void Draw();
	void DrawMesh();
	void DrawMeshWire() const;
	void DrawNormals() const;

	void CleanUp();

	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

	bool CheckMeshValidity();

	void LinkMat();

public:
	ResourceMesh* r_mesh;
	ResourceMesh* deformable_mesh;
	ComponentMaterial* c_mat;
	DGUID Material_Ind;
	uint mat_ind;
	AABB* BoundingBox;

public:
	void SetTexTo(const DGUID& nUID);
	void SetTexTo(ResourceMesh* nMesh);

	void SetBaseVals()
	{
		type = CT_Mesh;
		BoundingBox = nullptr;
		multiple = false;
		to_pop = false;
		mat_ind = -1;
		c_mat = nullptr;

		deformable_mesh = nullptr;
	}
};

#endif