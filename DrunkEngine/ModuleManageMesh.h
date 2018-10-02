#ifndef _LOAD_MESH_H_
#define _LOAD_MESH_H_

#include "Module.h"
#include "Assimp/include/scene.h"

struct v_data
{
	//aiMesh* m = nullptr;
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;
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

	void DrawMesh(const v_data* mesh);

public:
	//v_data test_mesh;
	std::vector<v_data> Meshes;

};



#endif