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