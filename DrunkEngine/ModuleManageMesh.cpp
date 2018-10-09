#include "ModuleManageMesh.h"
#include "Application.h"
#include "ConsoleWindow.h"
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
	LoadFBX("./BakerHouse.fbx");
	//LoadFBX("./warrior.fbx");

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

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast);// for better looks i guess: aiProcessPreset_TargetRealtime_MaxQuality);
	
	obj_data add_obj;
	std::string aux = file_path;
	
	add_obj.name = aux.substr(aux.find_last_of("\\") + 3);


	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			mesh_data add;
      
			SetColors(add, scene->mMeshes[i]);
      
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
			}
			PLOG("Said mesh starts with %d indices", add.num_index);


			


			SetTexCoords(&add, scene->mMeshes[i]);

			//add.tex_index = scene->mMeshes[i]->mMaterialIndex;

			GenBuffers(add);

			add_obj.meshes.push_back(add);
		}

		Objects.push_back(add_obj);
		
		// Set Parenting for later use
		std::vector<obj_data>::iterator item = --Objects.end();
		for (int k = 0; k < item->meshes.size(); k++)
			item->meshes[k].parent = item._Ptr;

		// Texture Setup
		if (scene->HasMaterials())
		{
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				SetupTex(*item._Ptr, true, scene->mMaterials[i]);
			}
		}
		else
			SetupTex(*item._Ptr);

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
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uvs);
			glTexCoordPointer(3, GL_FLOAT, 0, NULL);

			glBindTexture(GL_TEXTURE_2D, mesh->parent->textures[mesh->tex_index].id_tex);

		}
		else
			glColor3f(0, 0, 0);

		
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
		obj.mat_colors.push_back(float3(color.r,color.g,color.b));
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
				PLOG("Failed to load image from path %s", path.C_Str());
				texErr == aiReturn_FAILURE;
			}
			
			ilDeleteImages(1, &id_Image);

		}

		
	}
	
	if(texErr == aiReturn_FAILURE)// Load Checker texture
	{
		PLOG("Failed to load image: %s", iluErrorString(texErr));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, App->renderer3D->checkTexture);
	}
	// **Unbind Buffer**
	glBindTexture(GL_TEXTURE_2D, 0);
}

//-SET MESH DATA-----------------------------------------------------------------------------------------

bool ModuleManageMesh::SetTexCoords(mesh_data* mesh, aiMesh* cpy_data)
{
	bool ret = true;

	// Set TexCoordinates
	/*if (cpy_data->HasTextureCoords(0))
	{
		mesh->tex_coords = new float[mesh->num_vertex * 2];
		for (int i = 0; i < mesh->num_vertex; i++)
		{
			mesh->tex_coords[i * 2] = cpy_data->mTextureCoords[0][i].x;
			mesh->tex_coords[(i * 2) + 1] = cpy_data->mTextureCoords[0][i].y;
		}
	}
	else
		PLOG("No texture coordinates to be set");*/

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


void ModuleManageMesh::SetColors(mesh_data& mesh, aiMesh* cpy_data)
{

	// Color has maximum 8 channels
	for (int i = 0; i < 8; i++)
	{
		if (cpy_data->mColors[0] != nullptr)
		{
			mesh.mesh_color[i][0] = cpy_data->mColors[i]->r;
			mesh.mesh_color[i][1] = cpy_data->mColors[i]->g;
			mesh.mesh_color[i][2] = cpy_data->mColors[i]->b;
			mesh.mesh_color[i][3] = cpy_data->mColors[i]->a;
		}
		else
		{
			// Set color to white;
			mesh.mesh_color[i][0] = 1;
			mesh.mesh_color[i][1] = 1;
			mesh.mesh_color[i][2] = 1;
			mesh.mesh_color[i][3] = 1;
		}
	}

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



	glGenBuffers(1, (GLuint*) &(mesh.id_uvs));
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)mesh.id_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * mesh.num_uvs * 3, mesh.tex_coords, GL_STATIC_DRAW);

}