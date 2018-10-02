#include "ModuleManageMesh.h"
#include "GLEW/include/GL/glew.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include <gl/GL.h>
#include <gl/GLU.h>
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

	LoadFBX("./glass cube.fbx");

	return ret;
}

bool ModuleManageMesh::CleanUp()
{
	bool ret = false;

	// detach log stream
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

			PLOG("New mesh with %d vertices", add.num_vertex)
			if (scene->mMeshes[i]->HasFaces())
			{
				add.num_index = scene->mMeshes[i]->mNumFaces*3;
				add.index = new uint[add.num_index];

				for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
					{
						PLOG("WARNING, geometry face with != 3 indices!");
						ret = false; //if we want to stop load
					}
					else
					{
						memcpy(&add.index[i*3], scene->mMeshes[i]->mFaces[i].mIndices, 3*sizeof(uint));
					}
				}
			}

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