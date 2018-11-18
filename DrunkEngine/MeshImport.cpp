#include "MeshImport.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "InspectorWindow.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "ResourceMesh.h"
#include "Resource.h"

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

void MeshImport::LinkMesh(DGUID fID, ComponentMesh* mesh)
{
	MetaMesh* res = (MetaMesh*)App->resources->Library.at(fID);

	mesh->name = res->file;

	if (res->Asset.IsLoaded())
		res->Asset.LoadToMem();
	
	mesh->r_mesh = res->Asset.mesh.ptr;
	mesh->Material_Ind = res->Material_ind;
	mesh->mat_ind = res->mat_ind;
	res->UseCount++;

	mesh->SetMeshBoundBox();
	mesh->UID = fID;
}

ResourceMesh* MeshImport::LoadMesh(const char* file)
{
	ResourceMesh* ret = new ResourceMesh();

	std::ifstream read_file;
	read_file.open(file, std::fstream::in | std::fstream::binary);

	int size = read_file.seekg(0, read_file.end).tellg();
	read_file.seekg(0, read_file.beg);

	if (size > 1024)
	{
		char* data = new char[size];
		read_file.read(data, sizeof(char)*size);
		char* cursor = data;

		uint ranges[5];
		memcpy(ranges, cursor, sizeof(ranges));
		cursor += sizeof(ranges);

		ret->num_vertex = ranges[0] / 3;
		if ( ret->num_vertex)
		{
			ret->vertex = new GLfloat[ret->num_vertex * 3];
			memcpy(ret->vertex, cursor, ret->num_vertex * 3 * sizeof(GLfloat));
		}
		cursor += (ret->num_vertex * 3 * sizeof(GLfloat));

		ret->num_index = ranges[1];
		if (ret->num_index > 0)
		{
			ret->index = new GLuint[ret->num_index ];
			memcpy(ret->index, cursor, ret->num_index * sizeof(GLuint)); // Tried takign out the *3?
		}
		cursor += (ret->num_index * sizeof(GLuint));

		ret->num_normal = ranges[2] / 3;
		if (ret->num_normal > 0)
		{
			ret->normal = new GLfloat[ret->num_normal * 3];
			memcpy(ret->normal, cursor, ret->num_normal * 3 * sizeof(GLfloat));
		}

		cursor += (ret->num_normal * 3 * sizeof(GLfloat));

		ret->num_faces = ret->num_index / 3; // /2 when we save normals properly

		ret->num_uvs = ranges[3] / 3;
		if (ret->num_uvs > 0)
		{
			ret->tex_coords = new GLfloat[ret->num_uvs * 3];
			memcpy(ret->tex_coords, cursor, ret->num_uvs * sizeof(GLfloat) * 3);
		}

		cursor += (ranges[3] * sizeof(GLfloat));

		memcpy(&ret->def_color, cursor, sizeof(float) * ranges[4]);

		App->ui->console_win->AddLog("New mesh with %d vertices, %d indices, %d faces (tris)", ret->num_vertex, ret->num_index, ret->num_faces);

		ret->GenBuffers();
	}
	else
	{
		delete ret;
		ret = nullptr;
	}

	read_file.close();

	return ret;
}

////////////------------------------------------------------------------------------------------------------------------------
//-EXPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MeshImport::ExportAIMesh(const aiMesh* mesh, const int& mesh_id, const char* path)
{
	uint buf_size = 0;

	uint vertex_size = (mesh->mNumVertices * 3); 
	buf_size += sizeof(GLfloat)*vertex_size;
	uint index_size = 0;
	uint normal_size = 0;
	if (mesh->HasFaces())
	{
		index_size = (mesh->mNumFaces * 3); 
		buf_size += sizeof(GLuint)*index_size;
		normal_size = (mesh->mNumFaces * 2 * 3); 
		buf_size += sizeof(GLfloat)*normal_size;
	}
	
	uint uv_size = 0; 
	if(mesh->HasTextureCoords(0)) // We only load one UV channel for now
		uv_size = (mesh->mNumVertices * 3);
	buf_size += sizeof(GLfloat)*uv_size;

	uint BBox_size = 3 * 2; // 2 Vertex of 3 float each
	uint color_size = 4;
	uint size_size = sizeof(uint) * 5; // Amount of data put inside, the first values of data will be the size of each part

	buf_size += (size_size + sizeof(float)*BBox_size + sizeof(float) * color_size);

	char* data = new char[buf_size];
	char* cursor = data;

	uint ranges[5] =
	{
		vertex_size,
		index_size,
		normal_size,
		uv_size,
		color_size
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
		cursor += (sizeof(GLfloat)*vertex_size);
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
		cursor += (sizeof(GLuint)*index_size);
	}
	if (normal_aux.size() > 0)
	{
		memcpy(cursor, &normal_aux[0], sizeof(GLfloat)*normal_size);
		cursor += (sizeof(GLfloat)*normal_size);
	}

	if (uv_size > 0)
	{
		memcpy(cursor, &mesh->mTextureCoords[0][0], sizeof(GLfloat)*uv_size);
		cursor += (sizeof(GLfloat)*uv_size);
	}

	float color[4] = { 1,1,1,1 };
	aiColor4D* cpy = nullptr;
	for (int i = 0; i < 8 && cpy != NULL; i++)
		cpy = mesh->mColors[i];
	if (cpy != NULL)
	{
		color[0] = cpy->r;
		color[1] = cpy->g;
		color[2] = cpy->b;
		color[3] = cpy->a;
	}

	memcpy(cursor, &color[0], sizeof(float)*color_size);

	std::string filename = ".\\Library\\";
	filename += GetFileName(path) + "_Mesh_" + std::to_string(mesh_id);

	std::ofstream write_file;

	write_file.open((filename + ".meshdrnk").c_str(), std::fstream::out | std::fstream::binary);

	write_file.write(data, buf_size);

	write_file.close();

	ExportMeta(mesh, mesh_id, path);
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

void MeshImport::ExportMeta(const aiMesh* mesh, const int& mesh_id, std::string path)
{
	std::string meta_name = ".\\Library\\" + GetFileName(path.c_str()) + "_Mesh_" + std::to_string(mesh_id) + ".meta";
	JSON_Value* meta_file = json_parse_file(path.c_str());
	meta_file = json_value_init_object();

	JSON_Object* meta_obj = json_value_get_object(meta_file);

	json_object_dotset_string(meta_obj, "File", std::string(".\\Library\\"+ GetFileName(path.c_str()) + "_Mesh_" + std::to_string(mesh_id) + ".meshdrnk").c_str());
	std::string write = ".\\Library\\" + GetFileName(path.c_str()) + "_Mat_" + std::to_string(mesh->mMaterialIndex) + ".matdrnk";
	write = DGUID(write.c_str()).MD5ID;
	write[32] = '\0';
	json_object_dotset_string(meta_obj, "Material_Ind", write.c_str());
	json_object_dotset_number(meta_obj, "mat_ind", mesh->mMaterialIndex);
	
	json_serialize_to_file(meta_file, meta_name.c_str());
}
void MeshImport::LoadMeta(const char* file, MetaMesh * meta)
{
	meta->type = RT_Mesh;

	JSON_Value* meta_file = json_parse_file(file);
	JSON_Object* meta_obj = json_value_get_object(meta_file);

	meta->file = json_object_dotget_string(meta_obj, "File");
	meta->Material_ind = json_object_dotget_string(meta_obj, "Material_Ind");
	meta->mat_ind = json_object_dotget_number(meta_obj, "mat_ind");


}
////////////------------------------------------------------------------------------------------------------------------------
//-SaveDT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MeshImport::ExportMesh(const ComponentMesh* mesh)
{
	uint vertex_size = sizeof(GLfloat)*(mesh->r_mesh->num_vertex * 3);
	uint index_size = sizeof(GLuint)*(mesh->r_mesh->num_faces);
	uint normal_size = sizeof(GLfloat)*(mesh->r_mesh->num_faces);
	uint uv_size = sizeof(float)*(mesh->r_mesh->num_vertex * 3);
	uint BBox_size = sizeof(GLfloat) * 3 * 2; // 2 Vertex of 3 float each
	uint Mat_index = sizeof(unsigned int); // The material index 

	uint size_size = sizeof(uint) * 4; // Amount of data put inside, the first values of data will be the size of each part

	uint buf_size = size_size + vertex_size + index_size + normal_size + uv_size + BBox_size + Mat_index;

	char* data = new char[buf_size];
	char* cursor = data;

	uint ranges[4] =
	{
		vertex_size / sizeof(GLfloat),
		index_size / sizeof(GLuint),
		normal_size / sizeof(GLfloat),
		uv_size / sizeof(float)
	};

	memcpy(cursor, ranges, sizeof(ranges));
	cursor += sizeof(ranges);

	memcpy(cursor, mesh->r_mesh->vertex, vertex_size);
	cursor += vertex_size;

	memcpy(cursor, mesh->r_mesh->index, index_size);
	cursor += index_size;

	memcpy(cursor, mesh->r_mesh->normal, normal_size);
	cursor += normal_size;

	memcpy(cursor, mesh->r_mesh->tex_coords, uv_size);
	cursor += uv_size;

	memcpy(cursor, &mesh->Material_Ind, Mat_index);

	std::ofstream write_file;

	write_file.open(mesh->name.c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();
}