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
	void GenBuffers();
	void SetMeshBoundBox();
	
	void Draw();
	void DrawMesh();
	void DrawMeshWire();
	void DrawNormals();

	

	void CleanUp();

	void Load(JSON_Object* comp);
	void Save(JSON_Array* comps);

public:
	GLuint id_index; // index in VRAM
	GLuint num_index;
	GLuint* index;

	GLuint id_vertex; // unique vertex in VRAM
	GLuint num_vertex;
	GLfloat* vertex;

	GLuint id_uvs;
	GLuint num_uvs;
	GLfloat* tex_coords;

	GLuint id_normal;
	GLuint num_normal;
	GLfloat* normal;

	GLuint Material_Ind;

	GLuint num_faces;

	AABB* BoundingBox;

	GameObject* root;

public:
	void SetTextTo(const int& Mat_ind) { this->Material_Ind = Mat_ind; };

	void SetBaseVals()
	{
		type = CT_Mesh;
		multiple = true;

		id_index = id_normal = id_vertex = id_uvs = 0;
		num_faces = num_index = num_normal = num_uvs = num_vertex = 0;

		index = nullptr;
		normal = tex_coords = vertex = nullptr;
		BoundingBox = nullptr;
		parent = root = nullptr;
		multiple = false;

		to_pop = false;
	}
};

#endif