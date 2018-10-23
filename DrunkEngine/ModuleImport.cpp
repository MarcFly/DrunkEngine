#include "ModuleImport.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "GeoPropertiesWindow.h"



void CallLog(const char* str, char* usrData);

bool ModuleImport::Init()
{
	// DevIL initialization
	ilInit();
	iluInit();

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = CallLog;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleImport::CleanUp()
{
	return true;
}

GameObject * ModuleImport::ImportGameObject(const char* path, const aiScene* scene, const aiNode * obj_node, GameObject* par)
{
	GameObject* ret = new GameObject();

	ret->parent = par;
	ret->root = ret->parent;
	while (ret->root->parent != nullptr)
		ret->root = ret->root->parent;

	ret->name = obj_node->mName.C_Str();

	for (int i = 0; i < obj_node->mNumMeshes; i++)
	{
		std::string filename = "./Library/Meshes/";
		filename += GetFileName(path) + "_Mesh_" + std::to_string(obj_node->mMeshes[i]);
		filename.append(".drnk");
		ComponentMesh* aux = ImportMesh(filename.c_str(), ret);
		if (aux == nullptr)
		{
			ExportMesh(scene, obj_node->mMeshes[i],path);
			ImportMesh(filename.c_str(), ret);
		}
		
	}
	for (int i = 0; i < scene->mNumMaterials; i++)
		ret->materials.push_back(ImportMaterial(scene->mMaterials[i], ret));

	for (int i = 0; i < obj_node->mNumChildren; i++)
		ret->children.push_back(ImportGameObject(path, scene, obj_node->mChildren[i], ret));

	ret->transform = new ComponentTransform(&obj_node->mTransformation, ret);
	App->camera->SetToObj(ret, ret->SetBoundBox());

	return ret;
}

ComponentMaterial * ModuleImport::ImportMaterial(const aiMaterial * mat, GameObject* par)
{
	ComponentMaterial* ret = new ComponentMaterial();
	ret->parent = par;

	// Default Material Color
	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	ret->default_print = { color.r, color.g, color.b, 1 };

	// For future property things
	ret->NumProperties = mat->mNumProperties;
	ret->NumDiffTextures = mat->GetTextureCount(aiTextureType_DIFFUSE);

	for (int i = 0; i < ret->NumDiffTextures; i++)
	{
		aiString path;
		aiReturn err = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		Texture* add = App->importer->ImportTexture(path.C_Str(), ret);
		if (add != nullptr)
			ret->textures.push_back(add);
	}

	App->ui->console_win->AddLog("New Material with %d textures loaded.", ret->textures.size());

	return ret;
}

Texture * ModuleImport::ImportTexture(const char * path, ComponentMaterial* par)
{
	Texture* ret = new Texture;
	ret->mparent = par;

	bool check_rep = false;

	if (strrchr(path, '\\') != nullptr)
	{
		ret->filename = strrchr(path, '\\');
		ret->filename.substr(ret->filename.find_first_of("\\") + 3);
	}
	else
		ret->filename = path;

	Texture* test = par->CheckTexRep(ret->filename.c_str());

	if (test != nullptr)
		check_rep = true;

	if (!check_rep)
	{
		// Load Tex parameters and data to vram?
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &ret->id_tex);
		glBindTexture(GL_TEXTURE_2D, ret->id_tex);

		App->renderer3D->GenTexParams();

		ILuint id_Image;
		ilGenImages(1, &id_Image);
		ilBindImage(id_Image);

		bool check = ilLoadImage(path);

		if (!check)
		{
			// Basically if the direct load does not work, it will get the name of the file and load it from the texture folder if its there
			std::string new_file_path = path;
			new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

			new_file_path = App->mesh_loader->tex_folder + new_file_path;

			check = ilLoadImage(new_file_path.c_str());
		}

		if (check)
		{
			ILinfo Info;

			iluGetImageInfo(&Info);
			if (Info.Origin == IL_ORIGIN_UPPER_LEFT)
				iluFlipImage();

			check = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

			ret->width = Info.Width;
			ret->height = Info.Height;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->width, ret->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

			App->ui->console_win->AddLog("Loaded Texture from path %s, with size %d x %d", path, ret->width, ret->height);

		}
		else
		{
			App->ui->console_win->AddLog("Failed to load image from path %s", path);

			glDeleteTextures(1, &ret->id_tex);
			delete ret;
			ret = nullptr;
		}

		ilDeleteImages(1, &id_Image);

		glBindTexture(GL_TEXTURE_2D, 0);

		App->ui->geo_properties_win->CheckMeshInfo();
	}
	else
	{

		App->ui->console_win->AddLog("Setting Reference to already Loaded Texture...");
		ret = test;
		if (ret->mparent != par)
			ret->referenced_mats.push_back(par);
	}

	return ret;
}

ComponentMesh * ModuleImport::ImportMesh(const char* file, GameObject* par)
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
		ret->vertex = new GLfloat[ret->num_vertex * 3];
		memcpy(ret->vertex, cursor, ret->num_vertex * 3 * sizeof(GLfloat));
		cursor += ((ret->num_vertex * 3 )* sizeof(GLfloat));

		ret->num_index = ranges[1];
		ret->index = new GLuint[ret->num_index * 3];
		memcpy(ret->index, cursor, ret->num_index * 3 * sizeof(GLuint));
		cursor += ((ret->num_index)* sizeof(GLuint));

		ret->num_normal = ranges[2] / 3;
		ret->normal = new GLfloat[ret->num_normal * 3];
		memcpy(ret->normal,cursor, ret->num_normal * 3 * sizeof(GLfloat));
		cursor += ((ret->num_normal * 3 )* sizeof(GLfloat));

		ret->num_faces = ret->num_index / 3; // /2 when we save normals properly

		ret->num_uvs = ranges[3] / 3;
		ret->tex_coords = new GLfloat[ret->num_uvs * 3];
		memcpy(ret->tex_coords, cursor, ret->num_uvs * sizeof(float) * 3);
		cursor += (ret->num_uvs * 3 * sizeof(float));

		ret->SetMeshBoundBox();

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

void ModuleImport::ExportScene(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast);// for better looks i guess: aiProcessPreset_TargetRealtime_MaxQuality);
	std::string aux = file_path;

	if (scene == nullptr)
	{
		// Trying to load it from the scene folder
		std::string new_file_path = file_path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = App->mesh_loader->scene_folder + new_file_path;

		scene = aiImportFile(new_file_path.c_str(), aiProcessPreset_TargetRealtime_Fast);
		aux = new_file_path;
	}
	else
		App->ui->console_win->AddLog("Failed to Load from file %s", file_path);

	if (scene->HasMeshes())
		for (int i = 0; i < scene->mNumMeshes; i++)
			ExportMesh(scene, i, file_path);
	if(scene->HasMaterials())
		for(int i = 0; i < scene->mNumMaterials; i++)
			for (int j = 0; j < scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE); j++)
			{
				aiString path;
				aiReturn err = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, j, &path);
				ExportTexture(path.C_Str());
			}
}

void ModuleImport::ExportTexture(const char * path)
{

	ILuint id_Image;
	ilGenImages(1, &id_Image);
	ilBindImage(id_Image);

	bool check = ilLoadImage(path);

	if (!check)
	{
		// Basically if the direct load does not work, it will get the name of the file and load it from the texture folder if its there
		std::string new_file_path = path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = App->mesh_loader->tex_folder + new_file_path;

		check = ilLoadImage(new_file_path.c_str());
	}

	if (check)
	{
		ILinfo Info;

		iluGetImageInfo(&Info);
		if (Info.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		check = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		ILuint size;
		ILubyte *data;
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size = ilSaveL(IL_DDS, NULL, 0);

		if (size > 0)
		{
			data = new ILubyte[size];

			std::string export_path = "./Library/Textures/";
			export_path.append(GetFileName(path).c_str());
			export_path.append(".dds");

			if (ilSaveL(IL_DDS, data, size) > 0)
				ilSaveImage(export_path.c_str());
		}

		App->ui->console_win->AddLog("Exported Texture from path %s", path);

	}
	else
	{
		App->ui->console_win->AddLog("Failed to Export image from path %s", path);
	}

	ilDeleteImages(1, &id_Image);
	
}

void ModuleImport::SerializeSceneData()
{
	std::ofstream write_file;
	GameObject* root = App->mesh_loader->Root_Object;
	std::string filename = root->name.c_str();
	filename.append(".drk");
	write_file.open(filename.c_str());
	write_file << filename.c_str() << "{\n";

	{
		for(GameObject* obj : root->children)
			SerializeObjectData(obj, write_file);
	}

	write_file << "\n}\n";

	write_file.close();
}

void ModuleImport::SerializeObjectData(const GameObject * obj, std::ofstream& file)
{
	TabFile(ParCount(obj->parent), file);

	file << obj->name.c_str() << "{\n";

	for (GameObject* obj : obj->children)
		SerializeObjectData(obj, file);

	file << "\n}\n";
}

void CallLog(const char* str, char* usrData)
{
	App->ui->console_win->AddLog(str);
}

// -----------------

void ModuleImport::ExportMesh(const aiScene* scene, const int& mesh_id, const char* path)
{
	aiMesh* mesh = scene->mMeshes[mesh_id];

	unsigned int vertex_size = sizeof(GLfloat)*(mesh->mNumVertices * 3);
	unsigned int index_size = sizeof(GLuint)*(mesh->mNumFaces * 3);
	unsigned int normal_size = sizeof(GLfloat)*(mesh->mNumFaces * 3 * 2);
	unsigned int uv_size = sizeof(float)*(mesh->mNumVertices * 3);
	unsigned int BBox_size = sizeof(GLfloat) * 3 * 2; // 2 Vertex of 3 float each
	//unsigned int Mat_index = sizeof(unsigned int); // The material index 

	unsigned int size_size = sizeof(unsigned int) * 5; // Amount of data put inside, the first values of data will be the size of each part

	unsigned int buf_size = size_size + vertex_size + index_size + normal_size + uv_size + BBox_size;

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

	//ExportBBox(cursor, mesh->mNumVertices);

	std::ofstream write_file;
	std::string filename = "./Library/Meshes/";
	filename += GetFileName(path) + "_Mesh_" + std::to_string(mesh_id);
	filename.append(".drnk");

	write_file.open(filename.c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();

}

void ModuleImport::ExportIndexNormals(const int& ind, std::vector<GLfloat>& normals, std::vector<GLuint>& index, std::vector<GLfloat>& vertex)
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

void ModuleImport::ExportBBox(char * data, const int& num_vertex)
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

	memcpy(data, &min_x, sizeof(float)); data += sizeof(float);
	memcpy(data, &min_y, sizeof(float)); data += sizeof(float);
	memcpy(data, &min_z, sizeof(float)); data += sizeof(float);
	memcpy(data, &max_x, sizeof(float)); data += sizeof(float);
	memcpy(data, &max_y, sizeof(float)); data += sizeof(float);
	memcpy(data, &max_z, sizeof(float)); data += sizeof(float);
}