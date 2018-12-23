#ifndef _Res_Mesh_
#define _Res_Mesh_

#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "MeshImport.h"
#include "Resource.h"
#include "Color.h"
#include "ComponentTransform.h"

struct ResourceMaterial;
struct DGUID;

struct AnimMesh
{
	GLuint id_vertex = 0; // unique vertex in VRAM
	GLuint num_vertex = 0;
	GLfloat* vertex = nullptr;

	GLuint id_vert_normals = 0;
	GLfloat* vert_normals = nullptr;

	void GenBuffers();

	void SetValsFromMesh(const ResourceMesh* cpy);
	void SetFromBind(const AnimMesh* cpy);

	void UnloadBuffers();

	~AnimMesh();
};

struct ResourceMesh
{
	ResourceMesh() {};
	ResourceMesh(const ResourceMesh* cpy);

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

	GLuint id_vert_normals = 0;
	GLfloat* vert_normals = nullptr;

	ResourceMaterial* mat = nullptr;

	GLuint num_faces = 0;

	Color def_color = { 1,1,1,1 };

	AnimMesh* BindPose = nullptr;
	AnimMesh* DefMesh = nullptr;

	void GenBuffers();

	void UnloadBuffers();

	void UnloadMem();
	~ResourceMesh();
};

class MetaMesh : public MetaResource
{
public:
	MetaMesh() { type = RT_Mesh; };
	~MetaMesh() {};

	DGUID Material_ind;
	uint mat_ind;
	bool has_skeleton;

	// More Load Options
	void LoadMetaFile(const char* file);


};

#endif