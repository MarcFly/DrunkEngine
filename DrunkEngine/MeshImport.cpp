#include "MeshImport.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include <iostream>
#include <fstream>
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

void MeshImport::Init()
{

}

////////////------------------------------------------------------------------------------------------------------------------
//-IMPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

ComponentMesh * MeshImport::ImportMesh(const char* file, GameObject* par)
{
	ComponentMesh* ret = new ComponentMesh();

	//ret->

	ret->parent = par;
	ret->root = ret->parent->root;

	ret->name = file;

	std::ifstream read_file;
	read_file.open(file, std::ios::binary);

	std::streampos end = read_file.seekg(0, read_file.end).tellg();
	read_file.seekg(0, read_file.beg);

	if (end > 1024)
	{
		char* data = new char[end];
		read_file.read(data, sizeof(char)*end);

		char* cursor = data;

		unsigned int BBox_size = 0;

		uint ranges[5];
		memcpy(ranges, cursor, sizeof(ranges));
		cursor += sizeof(ranges);

		ret->num_vertex = ranges[0] / 3;
		if (ret->num_vertex)
		{
			ret->vertex = new GLfloat[ret->num_vertex * 3];
			memcpy(ret->vertex, cursor, ret->num_vertex * 3 * sizeof(GLfloat));
		}
		cursor += ((ret->num_vertex * 3) * sizeof(GLfloat));

		ret->num_index = ranges[1];
		if (ret->num_index > 0)
		{
			ret->index = new GLuint[ret->num_index * 3];
			memcpy(ret->index, cursor, ret->num_index * 3 * sizeof(GLuint));
		}
		cursor += ((ret->num_index) * sizeof(GLuint));

		ret->num_normal = ranges[2] / 3;
		if (ret->num_normal > 0)
		{
			ret->normal = new GLfloat[ret->num_normal * 3];
			memcpy(ret->normal, cursor, ret->num_normal * 3 * sizeof(GLfloat));
		}
		cursor += ((ret->num_normal * 3) * sizeof(GLfloat));

		ret->num_faces = ret->num_index; // /2 when we save normals properly

		ret->num_uvs = ranges[3] / 3;
		if (ret->num_uvs > 0)
		{
			ret->tex_coords = new GLfloat[ret->num_uvs * 3];
			memcpy(ret->tex_coords, cursor, ret->num_uvs * sizeof(GLfloat) * 3);
		}
		cursor += (ranges[3] * sizeof(GLfloat));

		float bbox[6]; // Bounding Box size always 6
		memcpy(bbox, cursor, sizeof(bbox));
		ret->BoundingBox = new AABB(vec(bbox[0], bbox[1], bbox[2]), vec(bbox[3], bbox[4], bbox[5]));
		cursor += sizeof(bbox);

		// Material Index always has 1, which should not be here but oke right now
		memcpy(&ret->Material_Ind, cursor, sizeof(unsigned int));

		ret->GenBuffers();

		App->ui->console_win->AddLog("New mesh with %d vertices, %d indices, %d faces (tris)", ret->num_vertex, ret->num_index, ret->num_faces);

		App->ui->geo_properties_win->CheckMeshInfo();

		par->meshes.push_back(ret);
	}
	else
	{
		delete ret;
		ret = nullptr;
	}
	return ret;
}

////////////------------------------------------------------------------------------------------------------------------------
//-EXPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MeshImport::ExportMesh(const aiScene* scene, const int& mesh_id, const char* path)
{
	aiMesh* mesh = scene->mMeshes[mesh_id];

	unsigned int vertex_size = sizeof(GLfloat)*(mesh->mNumVertices * 3);
	unsigned int index_size = sizeof(GLuint)*(mesh->mNumFaces * 3);
	unsigned int normal_size = sizeof(GLfloat)*(mesh->mNumFaces * 3 * 2);
	unsigned int uv_size = sizeof(float)*(mesh->mNumVertices * 3);
	unsigned int BBox_size = sizeof(GLfloat) * 3 * 2; // 2 Vertex of 3 float each
	unsigned int Mat_index = sizeof(unsigned int); // The material index 

	unsigned int size_size = sizeof(unsigned int) * 5; // Amount of data put inside, the first values of data will be the size of each part

	unsigned int buf_size = size_size + vertex_size + index_size + normal_size + uv_size + BBox_size + Mat_index;

	char* data = new char[buf_size];
	char* cursor = data;

	uint ranges[5] =
	{
		vertex_size / sizeof(GLfloat),
		index_size / sizeof(GLuint),
		normal_size / sizeof(GLfloat),
		uv_size / sizeof(float),
		BBox_size / sizeof(GLfloat)
	};

	memcpy(cursor, ranges, sizeof(ranges));
	cursor += sizeof(ranges);

	std::vector<GLfloat> vertex_aux;
	for (uint j = 0; j < mesh->mNumVertices; j++)
	{
		vertex_aux.push_back(mesh->mVertices[j].x);
		vertex_aux.push_back(mesh->mVertices[j].y);
		vertex_aux.push_back(mesh->mVertices[j].z);
	}
	memcpy(cursor, &vertex_aux[0], vertex_size);
	cursor += vertex_size;

	std::vector<GLuint> index_aux;
	std::vector<GLfloat> normal_aux;
	for (uint j = 0; j < mesh->mNumFaces; j++)
	{
		index_aux.push_back(mesh->mFaces[j].mIndices[0]);
		index_aux.push_back(mesh->mFaces[j].mIndices[1]);
		index_aux.push_back(mesh->mFaces[j].mIndices[2]);

		ExportIndexNormals(j, normal_aux, index_aux, vertex_aux);
	}
	memcpy(cursor, &index_aux[0], index_size);
	cursor += index_size;

	memcpy(cursor, &normal_aux[0], normal_size);
	cursor += normal_size;

	std::vector<float> uv_aux;
	for (uint j = 0; j < mesh->mNumVertices; j++)
	{
		uv_aux.push_back(mesh->mTextureCoords[0][j].x);
		uv_aux.push_back(mesh->mTextureCoords[0][j].y);
		uv_aux.push_back(mesh->mTextureCoords[0][j].z);
	}

	memcpy(cursor, &uv_aux[0], uv_size);
	cursor += uv_size;

	std::vector<float> test = ExportBBox(mesh->mVertices, mesh->mNumVertices);
	memcpy(cursor, &test[0], sizeof(float) * 6);
	cursor += sizeof(float) * 6;

	memcpy(cursor, &mesh->mMaterialIndex, Mat_index);

	std::ofstream write_file;
	std::string filename = "./Library/Meshes/";
	filename += App->importer->GetFileName(path) + "_Mesh_" + std::to_string(mesh_id);
	filename.append(".drnk");

	write_file.open(filename.c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();

}

void MeshImport::ExportIndexNormals(const int& ind, std::vector<GLfloat>& normals, std::vector<GLuint>& index, std::vector<GLfloat>& vertex)
{
	float aux[9];

	aux[0] = vertex[index[ind * 3] * 3];
	aux[1] = vertex[(index[ind * 3] * 3) + 1];
	aux[2] = vertex[(index[ind * 3] * 3) + 2];
	aux[3] = vertex[(index[(ind * 3) + 1] * 3)];
	aux[4] = vertex[(index[(ind * 3) + 1] * 3) + 1];
	aux[5] = vertex[(index[(ind * 3) + 1] * 3) + 2];
	aux[6] = vertex[(index[(ind * 3) + 2] * 3)];
	aux[7] = vertex[(index[(ind * 3) + 2] * 3) + 1];
	aux[8] = vertex[(index[(ind * 3) + 2] * 3) + 2];

	float p1 = (aux[0] + aux[3] + aux[6]) / 3;
	float p2 = (aux[1] + aux[4] + aux[7]) / 3;
	float p3 = (aux[2] + aux[5] + aux[8]) / 3;

	normals.push_back(p1);
	normals.push_back(p2);
	normals.push_back(p3);

	vec v1(aux[0], aux[1], aux[2]);
	vec v2(aux[3], aux[4], aux[5]);
	vec v3(aux[6], aux[7], aux[8]);

	vec norm = (v2 - v1).Cross(v3 - v1);
	norm.Normalize();

	normals.push_back(p1 + norm.x);
	normals.push_back(p2 + norm.y);
	normals.push_back(p3 + norm.z);
}

std::vector<float> MeshImport::ExportBBox(const aiVector3D* verts, const int& num_vertex)
{
	std::vector<float> ret;

	float max_x = INT_MIN, max_y = INT_MIN, max_z = INT_MIN, min_x = INT_MAX, min_y = INT_MAX, min_z = INT_MAX;

	for (int i = 0; i < num_vertex; i++)
	{
		if (max_x < verts[i].x)
			max_x = verts[i].x;
		if (min_x > verts[i].x)
			min_x = verts[i].x;
		if (max_y < verts[i].y)
			max_y = verts[i].y;
		if (min_y > verts[i].y)
			min_y = verts[i].y;
		if (max_z < verts[i].z)
			max_z = verts[i].z;
		if (min_z > verts[i].z)
			min_z = verts[i].z;
	}

	ret.push_back(min_x);
	ret.push_back(min_y);
	ret.push_back(min_z);
	ret.push_back(max_x);
	ret.push_back(max_y);
	ret.push_back(max_z);

	return ret;
}

////////////------------------------------------------------------------------------------------------------------------------
//-SaveDT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------
