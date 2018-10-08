#ifndef _LOAD_MESH_H_
#define _LOAD_MESH_H_

#include "Module.h"
#include "GLEW/include/GL/glew.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#include "Assimp/include/scene.h"
#include "Color.h"

struct mesh_data
{
	//aiMesh* m = nullptr;
	GLuint id_index = 0; // index in VRAM
	GLuint num_index = 0;
	GLuint* index = nullptr;

	GLuint id_vertex = 0; // unique vertex in VRAM
	GLuint num_vertex = 0;
	float* vertex = nullptr;

	float mesh_color[4];

	GLuint id_normal = 0;
	GLuint num_normal = 0;
	GLfloat* normal = nullptr;

	GLfloat* tex_coords = nullptr;

	GLuint id_tex = 0;

};

class ModuleManageMesh : public Module
{
public:
	ModuleManageMesh(Application* parent, bool start_enabled = true);
	~ModuleManageMesh() {};

	bool Init();
	bool Start();

	bool CleanUp();

	bool LoadFBX(const char* file_path);

	bool SetTexCoords(mesh_data* mesh, aiMesh* cpy_data);
	bool SetColors(mesh_data* mesh, aiMesh* cpy_data);

	void GenBuffers(mesh_data& mesh);
	void DrawMesh(const mesh_data* mesh, bool use_texture);
	void SetupTex(mesh_data& mesh);

public:
	//mesh_data test_mesh;
	std::vector<mesh_data> Meshes;

};



#endif