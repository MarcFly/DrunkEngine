#include "ModuleImport.h"

void ModuleImport::ExportMesh(const aiScene* scene, const int& mesh_id, const char* path)
{
	aiMesh* mesh = scene->mMeshes[mesh_id];

	unsigned int vertex_size = sizeof(float)*(mesh->mNumVertices * 3);
	unsigned int index_size = sizeof(GLuint)*(mesh->mNumFaces * 3);
	unsigned int normal_size = index_size * 2;
	unsigned int uv_size = vertex_size;
	unsigned int BBox_size = sizeof(float) * 3 * 2; // 2 Vertex of 3 float each
	unsigned int Mat_index = sizeof(unsigned int); // The material index 

	unsigned int buf_size = vertex_size + index_size + normal_size + uv_size + BBox_size + Mat_index;

	char* data = new char[buf_size];

	memcpy(&data[0], mesh->mVertices, vertex_size);
	for (uint j = 0; j < mesh->mNumFaces; j++)
	{
		memcpy(&data[vertex_size + j * 3], mesh->mFaces[j].mIndices, sizeof(GLuint) * 3);
		ExportMeshNormals(data, j, vertex_size, index_size);
	}

	memcpy(&data[vertex_size + index_size + normal_size], mesh->mTextureCoords[0], uv_size);

	ExportBBox(data, mesh->mNumVertices, vertex_size + index_size + normal_size + uv_size);

	memcpy(&data[buf_size - sizeof(unsigned int)], &mesh->mMaterialIndex, sizeof(unsigned int));

	std::ofstream write_file;
	std::string filename = "./Library/Meshes/";
	filename += GetFileName(path) + "_Mesh_" + std::to_string(mesh_id);
	filename.append(".drnk");

	write_file.open(filename.c_str(), std::ios::binary);

	write_file << data;

	write_file.close();

}

void ModuleImport::ExportMeshNormals(char * data, const int & index, const unsigned int& vertex_size, const unsigned int& index_size)
{
	float aux[9];

	aux[0] = data[data[vertex_size + index * 3] * 3];
	aux[1] = data[(data[vertex_size + index * 3] * 3) + 1];
	aux[2] = data[(data[vertex_size + index * 3] * 3) + 2];
	aux[3] = data[(data[vertex_size + (index * 3) + 1] * 3)];
	aux[4] = data[(data[vertex_size + (index * 3) + 1] * 3) + 1];
	aux[5] = data[(data[vertex_size + (index * 3) + 1] * 3) + 2];
	aux[6] = data[(data[vertex_size + (index * 3) + 2] * 3)];
	aux[7] = data[(data[vertex_size + (index * 3) + 2] * 3) + 1];
	aux[8] = data[(data[vertex_size + (index * 3) + 2] * 3) + 2];

	float p1 = (aux[0] + aux[3] + aux[6]) / 3;
	float p2 = (aux[1] + aux[4] + aux[7]) / 3;
	float p3 = (aux[2] + aux[5] + aux[8]) / 3;

	data[vertex_size + index_size + index * 6] = p1;
	data[vertex_size + index_size + index * 6 + 1] = p2;
	data[vertex_size + index_size + index * 6 + 2] = p3;

	vec v1(aux[0], aux[1], aux[2]);
	vec v2(aux[3], aux[4], aux[5]);
	vec v3(aux[6], aux[7], aux[8]);

	vec norm = (v2 - v1).Cross(v3 - v1);
	norm.Normalize();

	data[vertex_size + index_size + index * 6 + 3] = p1 + norm.x;
	data[vertex_size + index_size + index * 6 + 4] = p2 + norm.y;
	data[vertex_size + index_size + index * 6 + 5] = p3 + norm.z;
}

void ModuleImport::ExportBBox(char * data, const int& num_vertex, const unsigned int & start_size)
{
	float max_x = INT_MIN, max_y = INT_MIN, max_z = INT_MIN, min_x = INT_MAX, min_y = INT_MAX, min_z = INT_MAX;

	for (int i = 0; i < num_vertex; i++)
	{
		if (max_x < data[i * 3])
			max_x = data[i * 3];
		if (min_x > data[i * 3])
			min_x = data[i * 3];
		if (max_y < data[i * 3 + 1])
			max_y = data[i * 3 + 1];
		if (min_y > data[i * 3 + 1])
			min_y = data[i * 3 + 1];
		if (max_z < data[i * 3 + 2])
			max_z = data[i * 3 + 2];
		if (min_z > data[i * 3 + 2])
			min_z = data[i * 3 + 2];
	}

	memcpy(&data[start_size], &min_x, sizeof(float));
	memcpy(&data[start_size + 1], &min_y, sizeof(float));
	memcpy(&data[start_size + 2], &min_z, sizeof(float));
	memcpy(&data[start_size + 3], &max_x, sizeof(float));
	memcpy(&data[start_size + 4], &max_y, sizeof(float));
	memcpy(&data[start_size + 5], &max_z, sizeof(float));
}

std::string GenMeshName(const char* file, const int& mesh_id)
{
	std::string ret;



	return ret;
}