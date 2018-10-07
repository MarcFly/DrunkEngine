#include "ModuleManageMesh.h"
#include "Application.h"
//#include "ModuleRenderer3D.h"
#include "ConsoleWindow.h"
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

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			mesh_data add;
      
			if(scene->mMeshes[i]->mColors[0] != nullptr)
			{
				add.mesh_color[0] = scene->mMeshes[i]->mColors[0]->r;
				add.mesh_color[1] = scene->mMeshes[i]->mColors[0]->g;
				add.mesh_color[2] = scene->mMeshes[i]->mColors[0]->b;
				add.mesh_color[3] = scene->mMeshes[i]->mColors[0]->a;
			}
			else
			{
				add.mesh_color[0] = 1;
				add.mesh_color[1] = 1;
				add.mesh_color[2] = 1;
				add.mesh_color[3] = 1;
			}
      
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
					
						float aux[9];
						aux[0] = add.vertex[add.index[j * 3]];
						aux[1] = add.vertex[add.index[j * 3] + 1];
						aux[2] = add.vertex[add.index[j * 3] + 2];
						aux[3] = add.vertex[add.index[(j * 3) + 1]];
						aux[4] = add.vertex[add.index[(j * 3) + 1] + 1];
						aux[5] = add.vertex[add.index[(j * 3) + 1] + 2];
						aux[6] = add.vertex[add.index[(j * 3) + 2]];
						aux[7] = add.vertex[add.index[(j * 3) + 2] + 1];
						aux[8] = add.vertex[add.index[(j * 3) + 2] + 2];

						float v1 = (aux[0] + aux[3] + aux[6]) / 3;
						float v2 = (aux[1] + aux[4] + aux[7]) / 3;
						float v3 = (aux[2] + aux[5] + aux[8]) / 3;

						add.normal[j * 3 * 2] = v1;
						add.normal[(j * 3 * 2) + 1] = v2;
						add.normal[(j * 3 * 2) + 2] = v3;

						add.normal[(j * 3 * 2) + 3] = v1 + v1;
						add.normal[(j * 3 * 2) + 4] = v2 + v2;
						add.normal[(j * 3 * 2) + 5] = v3 + v3;
					}
				}
			}
			PLOG("Said mesh starts with %d indices", add.num_index)

			SetTexCoords(&add, scene->mMeshes[i]);

			GenBuffers(add);
      
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

bool ModuleManageMesh::SetTexCoords(mesh_data* mesh, aiMesh* cpy_data)
{
	bool ret = true;
	
	// Set TexCoordinates
	if (cpy_data->HasTextureCoords(0))
	{
		mesh->num_tex_coords = mesh->num_vertex;
		mesh->tex_coords = new float[mesh->num_tex_coords * 2];
		for (int i = 0; i < mesh->num_tex_coords; i++)
		{
			mesh->tex_coords[i*2] = cpy_data->mTextureCoords[0][i].x;
			mesh->tex_coords[i*2 + 1] = cpy_data->mTextureCoords[0][i].y;
		}
	}
	else
		PLOG("No texture coordinates to be set");


	return ret;
}

void ModuleManageMesh::DrawMesh(const mesh_data* mesh) 
{
	// Draw elements
	{
		
		glColor3f(1, 1, 0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Bind buffers
		glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
		glBindBuffer(GL_TEXTURE_COORD_ARRAY, mesh->id_tex_coords);
		glBindTexture(GL_TEXTURE_2D, mesh->id_tex);
		
		// Set pointers to arrays
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glTexCoordPointer(2, GL_FLOAT, 0, mesh->tex_coords);

		// Draw
		glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);

		// Unbind Buffers
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glColor3f(0, 1, 0);

		//Draw normals (buffer)
		//glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normal);
		//glVertexPointer(3, GL_FLOAT, 0, mesh->normal);
		//glDrawArrays(GL_LINE, 0, mesh->num_normal);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Draw Nornals
		//glBegin(GL_LINES);
		//glColor3f(1.0f, 1.0f, 1.0f);
		//
		//for (int i = 0; i < mesh->num_normal / 6; i++)
		//{			
		//	glVertex3f(mesh->normal[i * 6], mesh->normal[i * 6 + 1], mesh->normal[i * 6 + 2]);
		//	glVertex3f(mesh->normal[i * 6 + 3], mesh->normal[i * 6 + 4], mesh->normal[i * 6 + 5]);
		//}
		//
		//glEnd();

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		
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

	// Normal Buffer
	glGenBuffers(1, &mesh.id_normal);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * mesh.num_normal, mesh.normal, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Texture Coordinate Buffer
	glGenBuffers(1, &mesh.id_tex_coords);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, mesh.id_tex_coords);
	glBufferData(GL_TEXTURE_COORD_ARRAY, sizeof(float) * mesh.num_tex_coords * 2, mesh.tex_coords, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);

	// Texture 
	SetTexParams(&mesh);
}

bool ModuleManageMesh::SetColors(mesh_data* mesh, aiMesh* cpy_data)
{
	bool ret = true;

	return ret;
}

void ModuleManageMesh::SetTexParams(mesh_data* mesh)
{

	// Load Tex parameters and data to vram?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &mesh->id_tex);
	glBindTexture(GL_TEXTURE_2D, mesh->id_tex);

	// Texture Wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texture Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_HEIGHT, CHECKERS_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, App->renderer3D->checkTexture);

	// **Unbind Buffer**
	glBindTexture(GL_TEXTURE_2D, 0);

}