#include "MeshImport.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "ObjectPropertiesWindow.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include <fstream>
#include <iostream>
#include <experimental/filesystem>

void MeshImport::Init()
{
	// Will initate later values
}

////////////------------------------------------------------------------------------------------------------------------------
//-IMPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

ComponentMesh* MeshImport::ImportMesh(const char* file, ComponentMesh* mesh)
{
	App->importer->Imp_Timer.Start();


	mesh->name = file;

	std::ifstream read_file;
	read_file.open(mesh->name.c_str(), std::ios::in || std::ios::binary);
	
	std::streampos end = read_file.seekg(0, read_file.end).tellg();
	read_file.seekg(0, read_file.beg);

	if (end > 1024)
	{
		char* data = new char[end];
		read_file.read(data, sizeof(char)*end);
		char* cursor = data;

		uint ranges[5];
		memcpy(ranges, cursor, sizeof(ranges));
		cursor += sizeof(ranges);

		mesh->num_vertex = ranges[0] / 3;
		if (mesh->num_vertex)
		{
			mesh->vertex = new GLfloat[mesh->num_vertex * 3];
			memcpy(mesh->vertex, cursor, mesh->num_vertex * 3 * sizeof(GLfloat));
		}
		cursor += ((mesh->num_vertex * 3) * sizeof(GLfloat));

		mesh->num_index = ranges[1];
		if (mesh->num_index > 0)
		{
			mesh->index = new GLuint[mesh->num_index ];
			memcpy(mesh->index, cursor, mesh->num_index * sizeof(GLuint)); // Tried takign out the *3?
		}
		cursor += ((mesh->num_index) * sizeof(GLuint));

		mesh->num_normal = ranges[2] / 3;
		if (mesh->num_normal > 0)
		{
			mesh->normal = new GLfloat[mesh->num_normal * 3];
			memcpy(mesh->normal, cursor, mesh->num_normal * 3 * sizeof(GLfloat));
		}
		cursor += ((mesh->num_normal * 3) * sizeof(GLfloat));

		mesh->num_faces = mesh->num_index; // /2 when we save normals properly

		mesh->num_uvs = ranges[3] / 3;
		if (mesh->num_uvs > 0)
		{
			mesh->tex_coords = new GLfloat[mesh->num_uvs * 3];
			memcpy(mesh->tex_coords, cursor, mesh->num_uvs * sizeof(GLfloat) * 3);
		}
		cursor += (mesh->num_uvs * 3 * sizeof(GLfloat));
		
		float bbox[6]; // Bounding Box size always 6
		memcpy(bbox, cursor, sizeof(bbox));
		mesh->BoundingBox = new AABB(vec(bbox[0], bbox[1], bbox[2]), vec(bbox[3], bbox[4], bbox[5]));
		cursor += sizeof(bbox);

		// Material Index always has 1, which should not be here but oke right now
		memcpy(&mesh->Material_Ind, cursor, sizeof(unsigned int));

		mesh->GenBuffers();

		App->ui->console_win->AddLog("New mesh with %d vertices, %d indices, %d faces (tris)", mesh->num_vertex, mesh->num_index, mesh->num_faces);

		App->ui->obj_properties_win->CheckMeshInfo();
	}
	else
	{
		delete mesh;
		mesh = nullptr;
	}

	read_file.close();

	App->importer->Imp_Timer.LogTime("Mesh");

	return mesh;
}

////////////------------------------------------------------------------------------------------------------------------------
//-EXPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MeshImport::ExportMesh(const aiScene* scene, const int& mesh_id, const char* path)
{
	aiMesh* mesh = scene->mMeshes[mesh_id];

	uint buf_size = 0;

	uint vertex_size = (mesh->mNumVertices * 3); buf_size += sizeof(GLfloat)*vertex_size;
	uint index_size = 0;
	uint normal_size = 0;
	if (mesh->HasFaces())
	{
		index_size = (mesh->mNumFaces * 3); buf_size += sizeof(GLuint)*index_size;
		normal_size = (mesh->mNumFaces * 3 * 2); buf_size += sizeof(GLfloat)*normal_size;
	}
	
	uint uv_size = 0; 
	if(mesh->HasTextureCoords(0)) // We only load one UV channel for now
		uv_size = (mesh->mNumVertices * 3);
	buf_size += sizeof(GLfloat)*uv_size;

	uint BBox_size = 3 * 2; // 2 Vertex of 3 float each
	uint Mat_index = 0; // The material index 

	uint size_size = sizeof(uint) * 5; // Amount of data put inside, the first values of data will be the size of each part

	buf_size += size_size + sizeof(float)*BBox_size + sizeof(uint)/*MatIndex*/;

	char* data = new char[buf_size];
	char* cursor = data;

	uint ranges[5] =
	{
		vertex_size,
		index_size,
		normal_size,
		uv_size,
		BBox_size
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
	if (vertex_aux.size() > 0)
	{
		memcpy(cursor, &vertex_aux[0], sizeof(GLfloat)*vertex_size);
		cursor += sizeof(GLfloat)*vertex_size;
	}

	std::vector<GLuint> index_aux;
	std::vector<GLfloat> normal_aux;
	for (uint j = 0; j < mesh->mNumFaces; j++)
	{
		index_aux.push_back(mesh->mFaces[j].mIndices[0]);
		index_aux.push_back(mesh->mFaces[j].mIndices[1]);
		index_aux.push_back(mesh->mFaces[j].mIndices[2]);

		ExportIndexNormals(j, normal_aux, index_aux, vertex_aux);
	}
	if (index_aux.size() > 0)
	{
		memcpy(cursor, &index_aux[0], sizeof(GLuint)*index_size);
		cursor += sizeof(GLuint)*index_size;
	}
	if (normal_aux.size() > 0)
	{
		memcpy(cursor, &normal_aux[0], sizeof(GLfloat)*normal_size);
		cursor += sizeof(GLfloat)*normal_size;
	}

	std::vector<float> uv_aux;
	for (uint j = 0; j < uv_size/3; j++)
	{
		uv_aux.push_back(mesh->mTextureCoords[0][j].x);
		uv_aux.push_back(mesh->mTextureCoords[0][j].y);
		uv_aux.push_back(mesh->mTextureCoords[0][j].z);
	}

	if (uv_aux.size() > 0)
	{
		memcpy(cursor, &uv_aux[0], sizeof(GLfloat)*uv_size);
		cursor += sizeof(GLfloat)*uv_size;
	}

	std::vector<float> test = ExportBBox(mesh->mVertices, mesh->mNumVertices);
	float arr[6] = {test[0], test[1], test[2],test[3], test[4], test[5]};
	memcpy(cursor, &arr[0], sizeof(arr));
	cursor += sizeof(arr);

	memcpy(cursor, &mesh->mMaterialIndex, sizeof(uint));
	cursor += sizeof(uint);

	std::ofstream write_file;
	std::string filename = "./Library/Meshes/";
	filename += App->importer->GetFileName(path) + "_Mesh_" + std::to_string(mesh_id);
	filename.append(".meshdrnk");

	write_file.open(filename.c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();
}

void MeshImport::ExportIndexNormals(const int& ind, std::vector<GLfloat>& normals, std::vector<GLuint>& index, std::vector<GLfloat>& vertex)
{
	float aux[9];

	aux[0] = vertex[index[ind * 3] * 3];
	aux[1] = vertex[index[ind * 3] * 3 + 1];
	aux[2] = vertex[index[ind * 3] * 3 + 2];
	aux[3] = vertex[index[(ind * 3) + 1] * 3];
	aux[4] = vertex[index[(ind * 3) + 1] * 3 + 1];
	aux[5] = vertex[index[(ind * 3) + 1] * 3 + 2];
	aux[6] = vertex[index[(ind * 3) + 2] * 3];
	aux[7] = vertex[index[(ind * 3) + 2] * 3 + 1];
	aux[8] = vertex[index[(ind * 3) + 2] * 3 + 2];

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

	float max_x, max_y, max_z, min_x, min_y, min_z;
	max_x = max_y = max_z = INT_MIN;
	min_x = min_y = min_z = INT_MAX;


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

void MeshImport::ExportMesh(const ComponentMesh* mesh)
{
	uint vertex_size = sizeof(GLfloat)*(mesh->num_vertex * 3);
	uint index_size = sizeof(GLuint)*(mesh->num_faces);
	uint normal_size = sizeof(GLfloat)*(mesh->num_faces);
	uint uv_size = sizeof(float)*(mesh->num_vertex * 3);
	uint BBox_size = sizeof(GLfloat) * 3 * 2; // 2 Vertex of 3 float each
	uint Mat_index = sizeof(unsigned int); // The material index 

	uint size_size = sizeof(uint) * 5; // Amount of data put inside, the first values of data will be the size of each part

	uint buf_size = size_size + vertex_size + index_size + normal_size + uv_size + BBox_size + Mat_index;

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

	memcpy(cursor, mesh->vertex, vertex_size);
	cursor += vertex_size;

	memcpy(cursor, mesh->index, index_size);
	cursor += index_size;

	memcpy(cursor, mesh->normal, normal_size);
	cursor += normal_size;

	memcpy(cursor, mesh->tex_coords, uv_size);
	cursor += uv_size;

	std::vector<float> test;
	test.push_back(mesh->BoundingBox->minPoint.x);
	test.push_back(mesh->BoundingBox->minPoint.y);
	test.push_back(mesh->BoundingBox->minPoint.z);
	test.push_back(mesh->BoundingBox->maxPoint.x);
	test.push_back(mesh->BoundingBox->maxPoint.y);
	test.push_back(mesh->BoundingBox->maxPoint.z);

	memcpy(cursor, &test[0], sizeof(float) * 6);
	cursor += sizeof(float) * 6;

	memcpy(cursor, &mesh->Material_Ind, Mat_index);

	std::ofstream write_file;

	write_file.open(mesh->name.c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();
}