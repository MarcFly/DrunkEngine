#ifndef _COMPONENT_SKELETON_
#define _COMPONENT_SKELETON_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "Component.h"

class GameObject;
struct ResourceSkeleton;
struct Bone;

class ComponentSkeleton : public Component
{
public:
	ComponentSkeleton();
	ComponentSkeleton(GameObject* par);

	~ComponentSkeleton() {};

	void Update(const float dt);

	void Draw();
	void DrawDeformedMesh();
	void DeformMesh(std::vector<Bone*>& bones);
	void DrawToChildren(Bone* bone);
	
	void CleanUp();

	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

	float4x4 RecursiveParentInverted(float4x4& t, Bone& b);

	void UpdateTransform();

	bool CheckSkeletonValidity();
	//void LinkMesh();
	void CreateDeformableMesh();

public:
	ResourceSkeleton* r_skel;
	ComponentMesh* c_mesh;

private:
	float3 initial_pos;
	Quat initial_rot;
	float3 initial_scale;

	int check_vecs;

public:
	void SetBaseVals()
	{
		type = CT_Skeleton;
		multiple = false;
		to_pop = false;
		c_mesh = nullptr;
		r_skel = nullptr;
		check_vecs = 0;
	}
};

#endif