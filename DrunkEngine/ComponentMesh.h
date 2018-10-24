#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "Assimp/include/scene.h"

class GameObject;
class ComponentCamera;

class ComponentMesh
{
public:
	ComponentMesh();
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

	bool isMeshInsideFrustum(const ComponentCamera * cam, const AABB* bounding_box);

	void CleanUp();

public:
	std::string name;

	GLuint id_index = 0; // index in VRAM
	GLuint num_index = 0;
	GLuint* index = nullptr;

	GLuint id_vertex = 0; // unique vertex in VRAM
	GLuint num_vertex = 0;
	GLfloat* vertex = nullptr;

	GLuint id_uvs = 0;
	GLuint num_uvs = 0;
	GLfloat* tex_coords = nullptr;

	GLuint id_normal = 0;
	GLuint num_normal = 0;
	GLfloat* normal = nullptr;

	GLuint Material_Ind = 0;

	GLuint num_faces = 0;

	AABB* BoundingBox = nullptr;

	Primitive* BoundingBody = nullptr;

	GameObject* parent = nullptr;
	GameObject* root = nullptr;

	bool to_pop = false;

public:
	void SetTextTo(const int& Mat_ind) { this->Material_Ind = Mat_ind; };
};

#endif