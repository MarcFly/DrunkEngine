#ifndef _LOAD_MESH_H_
#define _LOAD_MESH_H_

#include "Module.h"
#include "GLEW/include/GL/glew.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#include "Assimp/include/scene.h"
#include "Color.h"
#include "Assimp/include/version.h"
#include "Primitive.h"

enum TexParams {
	Err = 0,

	TP_TEXTURE_COMPARE_MODE,
	TP_COMPARE_REF,
	TP_NONE,

	TP_TEXTURE_LOD_BIAS,

	TP_TEXTURE_FILTERS, // Filters start here, mag and min types
	TP_NEAREST,
	TP_LINEAR,
	TP_NEAREST_MIPMAP_NEAREST,
	TP_LINEAR_MIPMAP_NEAREST,
	TP_NEAREST_MIPMAP_LINEAR,
	TP_LINEAR_MIPMAP_LINEAR,

	TP_TEXTURE_MIN_LOD,
	TP_TEXTURE_MAX_LOD,
	TP_TEXTURE_MAX_LEVEL,

	// Wraps
	TP_TEXTURE_WRAP, // Wraps start here

	// Wrap Modes
	TP_CLAMP_TO_EDGE,	// coordinades clamped to range based on texture size
	TP_CLAMP_TO_BORDER,	// similar to edge but in borders data is set by border color
	TP_MIRRORED_REPEAT,	// coordinates set to the fractional part if goes beyond 1, so 1.1 = 1/1.1
	TP_REPEAT,			// Integer of coordinates ignored, creatign repetition pattern after 1.0, 1.1 = 0.1
	TP_MIRROR_CLAMP_TO_EDGE,	// repeat for 1 more int (until 2) then clamps to edge


};

struct obj_data;

struct texture_data
{
	GLuint id_tex = 0;
	GLuint width, height;
	std::string filename;
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
	GLfloat* tex_coords = nullptr;

	GLuint id_normal = 0;
	GLuint num_normal = 0;
	GLfloat* normal = nullptr;	

	GLuint tex_index = 0;

	GLuint num_faces = 0;

	obj_data* parent = nullptr;

	aiVector3D transform_position;
	aiVector3D transform_scale;
	aiVector3D transform_rotate;

	float box_x, box_nx, box_y, box_ny, box_z, box_nz;

};

struct obj_data
{
	std::string name;

	std::vector<mesh_data> meshes;

	std::vector<Color> mat_colors;
	std::vector<texture_data> textures;

	aiVector3D transform_position;
	aiVector3D transform_scale;
	aiQuaternion transform_rotate;

	Primitive* mathbody = nullptr; // In case we create a premade object // Temporary solution
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

	bool CreatePrimitiveObject(const vec& center, PCube& sphere);
	//bool CreatePrimitiveObject(const vec& center, PSphere& sphere);
	//bool CreatePrimitiveObject(const vec& center, PSphere& sphere);

	bool SetTexCoords(mesh_data* mesh, aiMesh* cpy_data);
	void SetNormals(mesh_data& mesh, const int& ind_value);
	void GenBuffers(mesh_data& mesh);

	void SetupMat(obj_data& mesh, aiMaterial* material = nullptr);
	bool LoadTextCurrentObj(const char* path, obj_data* curr_obj);
	void DestroyObject(const int& index);

	void GenTexParams();
	void SetCurrParams();

	void DrawMesh(const mesh_data* mesh, bool use_texture);

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

public:
	std::vector<obj_data> Objects;

	int curr_tws;
	int curr_twt;
	int curr_tmagf;
	int curr_tminf;

	std::string scene_folder;
	std::string tex_folder;

private:
	uint tws, twt, tmagf, tminf;

public:
	std::vector<obj_data> getObjects() const { return Objects; }

	int GetDevILVer();

	int GetAssimpMajorVer() { return aiGetVersionMajor(); };
	int GetAssimpMinorVer() { return aiGetVersionMinor(); };
	int GetAssimpVersionRevision() { return aiGetVersionRevision(); };

	void SetParents() 
	{
		for (int j = 0; j < Objects.size(); j++)
			for (int k = 0; k < Objects[j].meshes.size(); k++)
				Objects[j].meshes[k].parent = &Objects[j];
	}
};

#endif