#ifndef _MESH_IMPORT_
#define _MESH_IMPORT_

#include "Assimp/include/scene.h"
#include "GLEW/include/GL/glew.h"
#include <vector>

class ComponentMesh;
class GameObject;

class MeshImport {
public:
	MeshImport() {};
	~MeshImport() {};

	void Init();

	ComponentMesh* ImportMesh(const char* file, ComponentMesh* mesh);
	void LoadMesh(ComponentMesh* mesh);

	void ExportMesh(const aiScene* scene, const int& mesh_id, const char* path = nullptr);
	void ExportMesh(const ComponentMesh* mesh);

	void ExportIndexNormals(const int& ind, std::vector<GLfloat>& normals, std::vector<GLuint>& index, std::vector<GLfloat>& vertex);
	std::vector<float> ExportBBox(const aiVector3D* verts, const int& num_vertex);

};

#endif