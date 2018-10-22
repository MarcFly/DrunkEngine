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

GameObject * ModuleImport::ImportGameObject(const aiScene* scene, const aiNode * obj_node, GameObject* par)
{
	GameObject* ret = new GameObject();

	ret->parent = par;
	ret->root = ret->parent;
	while (ret->root->parent != nullptr)
		ret->root = ret->root->parent;

	ret->name = obj_node->mName.C_Str();

	for (int i = 0; i < obj_node->mNumMeshes; i++)
		ret->meshes.push_back(ImportMesh(scene->mMeshes[obj_node->mMeshes[i]], ret));

	for (int i = 0; i < scene->mNumMaterials; i++)
		ret->materials.push_back(ImportMaterial(scene->mMaterials[i], ret));

	for (int i = 0; i < obj_node->mNumChildren; i++)
		ret->children.push_back(ImportGameObject(scene, obj_node->mChildren[i], ret));

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

ComponentMesh * ModuleImport::ImportMesh(const aiMesh * mesh, GameObject* par)
{
	ComponentMesh* ret = new ComponentMesh();

	ret->parent = par;
	ret->root = ret->parent->root;

	ret->num_vertex = mesh->mNumVertices;
	ret->vertex = new float[ret->num_vertex * 3];
	ret->num_faces = mesh->mNumFaces;

	ret->name = mesh->mName.C_Str();

	memcpy(ret->vertex, mesh->mVertices, 3 * sizeof(float)*ret->num_vertex);

	if (mesh->HasFaces())
	{
		ret->num_index = mesh->mNumFaces * 3;
		ret->index = new GLuint[ret->num_index];

		ret->num_normal = ret->num_index * 2;
		ret->normal = new float[ret->num_normal];

		for (uint j = 0; j < mesh->mNumFaces; j++)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
				App->ui->console_win->AddLog("WARNING, geometry face with != 3 indices!");
			else
			{
				memcpy(&ret->index[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(GLuint));

				ret->SetNormals(j);
			}
		}

		ret->SetMeshBoundBox();
	}

	ret->SetTexCoords(mesh);

	ret->Material_Ind = mesh->mMaterialIndex;

	ret->GenBuffers();

	App->ui->console_win->AddLog("New mesh with %d vertices, %d indices, %d faces (tris)", ret->num_vertex, ret->num_index, ret->num_faces);

	App->ui->geo_properties_win->CheckMeshInfo();

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
			ExportMesh(scene->mMeshes[i]);
	if(scene->HasMaterials())
		for(int i = 0; i < scene->mNumMaterials; i++)
			for (int j = 0; j < scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE); j++)
			{
				//Export Textures
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
			std::string t_path = path;
			t_path = t_path.substr(t_path.find_last_of("\\/") + 1);
			export_path.append(t_path.erase(t_path.length() - 4).c_str());
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

void ModuleImport::ExportMesh(const aiMesh* mesh)
{
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

	memcpy(&data[buf_size - sizeof(unsigned int)] , &mesh->mMaterialIndex, sizeof(unsigned int));

	std::ofstream write_file;
	std::string filename = "./Library/Meshes/";
	filename += mesh->mName.C_Str();
	filename.append(".drnk");

	write_file.open(filename.c_str(), std::ios::binary);

	write_file << data;

	write_file.close();

}

void ModuleImport::ExportMeshNormals(char * data, const int & index, const unsigned int& vertex_size, const unsigned int& index_size)
{
	float aux[9];

	aux[0] = data[ data[vertex_size + index * 3] * 3];
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