#include "ModuleManageMesh.h"

#include "SDL/include/SDL_opengl.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

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

	return ret;
}

bool ModuleManageMesh::Start()
{
	bool ret = true;

	//LoadFBX("./glass cube.fbx");
	//LoadFBX("./Dragon 2.5_fbx.fbx");
	//LoadFBX("./Toilet.fbx");
	LoadFBX("./warrior.fbx");

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

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			v_data add;
			add.num_vertex = scene->mMeshes[i]->mNumVertices;
			add.vertex = new float[add.num_vertex*3];

			memcpy(add.vertex, scene->mMeshes[i]->mVertices, 3*sizeof(float)*add.num_vertex);

			// Properly Binding Buffer 1 TIME
			glGenBuffers(1, &add.id_vertex);
			glBindBuffer(GL_ARRAY_BUFFER, add.id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * add.num_vertex, add.vertex, GL_STATIC_DRAW);
			
			// **Unbind Buffer**
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			PLOG("New mesh with %d vertices", add.num_vertex)

			if (scene->mMeshes[i]->HasFaces())
			{
				add.num_index = scene->mMeshes[i]->mNumFaces*3;
				add.index = new GLuint[add.num_index];

				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						PLOG("WARNING, geometry face with != 3 indices!");
						ret = false; //if we want to stop load
					}
					else
					{
						memcpy(&add.index[i*3], scene->mMeshes[i]->mFaces[i].mIndices, 3*sizeof(GLuint));
					}
				}
			}
			PLOG("Said mesh starts with %d indices", add.num_index)
			// Properly Generating the buffer for
			glGenBuffers(1, &add.id_index);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, add.id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * add.num_index, add.index, GL_STATIC_DRAW);
			
			// **Unbind Buffer**
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			Meshes.push_back(add);
		}

		aiReleaseImport(scene);
	}
	else
	{
		PLOG("Error loading scene %s", file_path);
		ret = false;
	}

	return ret;
}

void ModuleManageMesh::DrawMesh(const v_data* mesh) 
{
	
}