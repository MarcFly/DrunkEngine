#ifndef _LOAD_MESH_H_
#define _LOAD_MESH_H_

#include "Module.h"
#include "GLEW/include/GL/glew.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#include "Assimp/include/scene.h"
#include "Color.h"
#include "Assimp/include/version.h"
struct obj_data;

struct texture_data
{
	GLuint id_tex = 0;
	GLuint width, height;
};

struct mesh_data
{
	std::string name;

	//aiMesh* m = nullptr;
	GLuint id_index = 0; // index in VRAM
	GLuint num_index = 0;
	GLuint* index = nullptr;

	GLuint id_vertex = 0; // unique vertex in VRAM
	GLuint num_vertex = 0;
	float* vertex = nullptr;

	GLuint id_uvs = 0;
	GLuint num_uvs = 0;

	float mesh_color[8][4];

	GLuint id_normal = 0;
	GLuint num_normal = 0;
	GLfloat* normal = nullptr;

	GLfloat* tex_coords = nullptr;

	GLuint tex_index = 0;

	obj_data* parent = nullptr;

};

struct obj_data
{
	std::string name;

	std::vector<mesh_data> meshes;

	std::vector<float3> mat_colors;
	std::vector<texture_data> textures;
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
	void SetNormals(mesh_data& mesh, const int& ind_value);
	void SetColors(mesh_data& mesh, aiMesh* cpy_data);
	void GenBuffers(mesh_data& mesh);

	void SetupTex(obj_data& mesh, bool has_texture = false, aiMaterial* material = nullptr);

	void DrawMesh(const mesh_data* mesh, bool use_texture);

public:
	//mesh_data test_mesh;
	std::vector<obj_data> Objects;

public:
	std::vector<obj_data> getObjects() const { return Objects; }

	int GetDevILVer();

	int GetAssimpMajorVer() { return aiGetVersionMajor(); };
	int GetAssimpMinorVer() { return aiGetVersionMinor(); };
	int GetAssimpVersionRevision() { return aiGetVersionRevision(); };

};

#endif