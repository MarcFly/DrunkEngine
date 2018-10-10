#include "ModuleManageMesh.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "ModuleCamera3D.h"
#include "SDL/include/SDL_opengl.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"


#include "ModuleRenderer3D.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "DevIL/libx86/Release/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/Release/ILU.lib")
#pragma comment (lib, "DevIL/libx86/Release/ILUT.lib")

ModuleManageMesh::ModuleManageMesh(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

bool ModuleManageMesh::Init()
{
	bool ret = true;

	// DevIL initialization
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	return ret;
}

bool ModuleManageMesh::Start()
{
	bool ret = true;

	//LoadFBX("./glass cube.fbx");
	//LoadFBX("./Dragon 2.5_fbx.fbx");
	//LoadFBX("./Toilet.fbx");
	//LoadFBX("./BakerHouse.fbx");
	//LoadFBX("./warrior.fbx");
	//LoadFBX("./KSR-29 sniper rifle new_fbx_7.4_binary.fbx");

	return ret;
}

bool ModuleManageMesh::CleanUp()
{
	bool ret = false;

	// detach log streamW
	aiDetachAllLogStreams();

	return ret;
}

bool ModuleManageMesh::LoadFBX(const char* file_path)
{
	bool ret = true;

	if (Objects.size() > 0)
		DestroyObject(0);

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast);// for better looks i guess: aiProcessPreset_TargetRealtime_MaxQuality);
	
	obj_data add_obj;
	std::string aux = file_path;
	
	add_obj.name = aux.substr(aux.find_last_of("\\") + 1);


	if (scene != nullptr && scene->HasMeshes())
	{

		float vertex_aux = 0.f;

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			mesh_data add;
      
			add.num_vertex = scene->mMeshes[i]->mNumVertices;
			add.vertex = new float[add.num_vertex*3];

			memcpy(add.vertex, scene->mMeshes[i]->mVertices, 3*sizeof(float)*add.num_vertex);

			PLOG("New mesh with %d vertices", add.num_vertex)

			if (scene->mMeshes[i]->HasFaces())
			{
				add.num_index = scene->mMeshes[i]->mNumFaces*3;
				add.index = new GLuint[add.num_index];

				add.num_normal = add.num_index * 2;
				add.normal = new float[add.num_normal];

				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						PLOG("WARNING, geometry face with != 3 indices!");
						ret = false; //if we want to stop load
					}
					else
					{
						memcpy(&add.index[j*3], scene->mMeshes[i]->mFaces[j].mIndices, 3*sizeof(GLuint));
					
						SetNormals(add, j);						
					}
				}			

				for (uint j = 0; j < scene->mMeshes[i]->mNumVertices * 3; j++)
				{
					if (vertex_aux < abs(add.vertex[j]))
						vertex_aux = abs(add.vertex[j]);
				}
			}
			PLOG("Said mesh starts with %d indices", add.num_index);

			SetTexCoords(&add, scene->mMeshes[i]);

			add.tex_index = scene->mMeshes[i]->mMaterialIndex;

			GenBuffers(add);

			add_obj.meshes.push_back(add);
		}


		if (Objects.size() > 0)
		{
			Objects.insert(Objects.begin(), add_obj);
			Objects.pop_back();
		}
		else
			Objects.push_back(add_obj);

		App->camera->Transport(vec3(vertex_aux + 3, vertex_aux + 3, vertex_aux + 3));
		App->camera->LookAt(vec3(0.0f, 0.0f, 0.0f));
		App->camera->mesh_multiplier = vertex_aux / 4;

		// ReSet all Parenting for later use
		for (int j = 0; j < Objects.size(); j++)
			for (int k = 0; k < Objects[j].meshes.size(); k++)
				Objects[j].meshes[k].parent = &Objects[j];

		// Texture Setup
		std::vector<obj_data>::iterator item = --Objects.end();
		if (scene->HasMaterials())
		{
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				SetupTex(*item._Ptr, true, scene->mMaterials[i]);
			}
		}

		/*if (item->textures.size() == 0) {
			SetupTex(*item._Ptr);
			for (int i = 0; i < item->meshes.size(); i++)
				if(item->meshes[i].tex_index > item->textures.size() - 1)
					item->meshes[i].tex_index = 0;
		}*/

		aiReleaseImport(scene);
	}
	else
	{
		PLOG("Error loading scene's meshes %s", file_path);
		ret = false;
	}

	// Texture Setup

	return ret;
}



void ModuleManageMesh::DrawMesh(const mesh_data* mesh, bool use_texture) 
{
	// Draw elements
	{

		glColor4f(1, 1, 1, 1);

		// Bind buffers
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);

		if (use_texture)
		{
			int test = mesh->parent->textures.size() - 1;
			if(mesh->parent->textures.size() > 0 && mesh->tex_index <= (mesh->parent->textures.size() - 1))
			{ 
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uvs);
				glTexCoordPointer(3, GL_FLOAT, 0, NULL);

				glBindTexture(GL_TEXTURE_2D, mesh->parent->textures[mesh->tex_index].id_tex);
			}
			else
			{
				Color c = mesh->parent->mat_colors[mesh->tex_index];
				glColor4f(c.r, c.g, c.b, c.a);
			}
		}
		else
		{
			glColor4f(1,1,1,1);
		}

		
		// Draw
		glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);

		// Unbind Buffers
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glColor4f(0, 1, 0, 1);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

	}
}

//-SET OBJ DATA------------------------------------------------------------------------------------------

void ModuleManageMesh::SetupTex(obj_data& obj, bool has_texture, aiMaterial* material)
{
	obj.textures.push_back(texture_data());
	texture_data* item = (--obj.textures.end())._Ptr;
	// Load Tex parameters and data to vram?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &item->id_tex);
	glBindTexture(GL_TEXTURE_2D, item->id_tex);

	// Texture Wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texture Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	aiReturn texErr;

	if (has_texture) // Load a proper texture
	{
		aiColor3D color;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		obj.mat_colors.push_back(Color(color.r,color.g,color.b,1));
		aiString path;
		texErr = material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

		if (texErr == aiReturn_SUCCESS)
		{
			
			ILuint id_Image;
			ilGenImages(1, &id_Image);
			ilBindImage(id_Image);

			bool check = ilLoadImage(path.C_Str());

			if (check)
			{
				ILinfo Info;

				iluGetImageInfo(&Info);
				if (Info.Origin == IL_ORIGIN_UPPER_LEFT)
					iluFlipImage();

				check = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

				item->width = Info.Width;
				item->height = Info.Height;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, item->width, item->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
			}
			else
			{
				PLOG("Failed to load image from path %s\n", path.C_Str());
				obj.textures.pop_back();
			}

			ilDeleteImages(1, &id_Image);

		}
		else
		{
			PLOG("Failed to load image from path %s\n", path.C_Str());
			obj.textures.pop_back();
		}
		
	}
	// **Unbind Buffer**
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool ModuleManageMesh::LoadTextCurrentObj(const char* path, obj_data* curr_obj)
{
	bool ret = true;

	curr_obj->textures.push_back(texture_data());
	texture_data* item = (--curr_obj->textures.end())._Ptr;

	// Load Tex parameters and data to vram?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &item->id_tex);
	glBindTexture(GL_TEXTURE_2D, item->id_tex);

	// Texture Wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texture Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	ILuint id_Image;
	ilGenImages(1, &id_Image);
	ilBindImage(id_Image);

	bool check = ilLoadImage(path);

	if (check)
	{
		ILinfo Info;

		iluGetImageInfo(&Info);
		if (Info.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		check = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		item->width = Info.Width;
		item->height = Info.Height;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, item->width, item->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		
		for (int i = 0; i < curr_obj->meshes.size(); i++)
		{
			curr_obj->meshes[i].tex_index = curr_obj->textures.size() - 1;
		}
	}
	else
	{
		PLOG("Failed to load image from path %s", path);
		curr_obj->textures.pop_back();
		//glDeleteTextures(1, &item->id_tex);
		ret = false;
	}

	

	ilDeleteImages(1, &id_Image);

	glBindTexture(GL_TEXTURE_2D, 0);

	return ret;
}

//-SET MESH DATA-----------------------------------------------------------------------------------------

bool ModuleManageMesh::SetTexCoords(mesh_data* mesh, aiMesh* cpy_data)
{
	bool ret = true;

	// Set TexCoordinates
	if (cpy_data->HasTextureCoords(0))
	{
		mesh->num_uvs = mesh->num_vertex;
		mesh->tex_coords = new float[mesh->num_uvs * 3];
		memcpy(mesh->tex_coords, cpy_data->mTextureCoords[0], mesh->num_uvs * sizeof(float) * 3);
	}
	else
		PLOG("No texture coordinates to be set");

	return ret;
}

void ModuleManageMesh::SetNormals(mesh_data& mesh, const int& ind_value)
{
	float aux[9];

	aux[0] = mesh.vertex[mesh.index[ind_value * 3] * 3];
	aux[1] = mesh.vertex[(mesh.index[ind_value * 3] * 3) + 1];
	aux[2] = mesh.vertex[(mesh.index[ind_value * 3] * 3) + 2];
	aux[3] = mesh.vertex[(mesh.index[(ind_value * 3) + 1] * 3)];
	aux[4] = mesh.vertex[(mesh.index[(ind_value * 3) + 1] * 3) + 1];
	aux[5] = mesh.vertex[(mesh.index[(ind_value * 3) + 1] * 3) + 2];
	aux[6] = mesh.vertex[(mesh.index[(ind_value * 3) + 2] * 3)];
	aux[7] = mesh.vertex[(mesh.index[(ind_value * 3) + 2] * 3) + 1];
	aux[8] = mesh.vertex[(mesh.index[(ind_value * 3) + 2] * 3) + 2];

	float p1 = (aux[0] + aux[3] + aux[6]) / 3;
	float p2 = (aux[1] + aux[4] + aux[7]) / 3;
	float p3 = (aux[2] + aux[5] + aux[8]) / 3;

	mesh.normal[ind_value * 6] = p1;
	mesh.normal[ind_value * 6 + 1] = p2;
	mesh.normal[ind_value * 6 + 2] = p3;

	vec v1(aux[0], aux[1], aux[2]);
	vec v2(aux[3], aux[4], aux[5]);
	vec v3(aux[6], aux[7], aux[8]);

	vec norm = (v2 - v1).Cross(v3 - v1);
	norm.Normalize();

	mesh.normal[ind_value * 6 + 3] = p1 + norm.x;
	mesh.normal[ind_value * 6 + 4] = p2 + norm.y;
	mesh.normal[ind_value * 6 + 5] = p3 + norm.z;
}

void ModuleManageMesh::GenBuffers(mesh_data& mesh)
{

	// Vertex Buffer
	glGenBuffers(1, &mesh.id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.num_vertex * 3, mesh.vertex, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Index Buffer
	glGenBuffers(1, &mesh.id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.num_index, mesh.index, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// Texture Coordinates / UVs Buffer
	glGenBuffers(1, (GLuint*) &(mesh.id_uvs));
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)mesh.id_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * mesh.num_uvs * 3, mesh.tex_coords, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// -----------------------
int ModuleManageMesh::GetDevILVer()
{
	return IL_VERSION;
}

void ModuleManageMesh::DestroyObject(const int& index)
{
	for (int i = 0; i < Objects[index].meshes.size(); i++) {
		glDeleteBuffers(1, &Objects[index].meshes[i].id_index);
		glDeleteBuffers(1, &Objects[index].meshes[i].id_normal);
		glDeleteBuffers(1, &Objects[index].meshes[i].id_uvs);
		glDeleteBuffers(1, &Objects[index].meshes[i].id_vertex);

		delete Objects[index].meshes[i].index;
		delete Objects[index].meshes[i].normal;
		delete Objects[index].meshes[i].tex_coords;
		delete Objects[index].meshes[i].vertex;
	}
	Objects[index].meshes.clear();

	for (int i = 0; i < Objects[index].textures.size(); i++)
	{
		glDeleteTextures(1, &Objects[index].textures[i].id_tex);
	}
	Objects[index].textures.clear();

	Objects[index].mat_colors.clear();

	Objects.erase(Objects.begin(), Objects.begin());
}