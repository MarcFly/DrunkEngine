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
#include "GeoPropertiesWindow.h"


#include "ModuleRenderer3D.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "DevIL/libx86/Release/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/Release/ILU.lib")

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

	return ret;
}

bool ModuleManageMesh::Start()
{
	bool ret = true;

	Load(nullptr);
	LoadFBX("./BakerHouse.fbx");

	return ret;
}

bool ModuleManageMesh::CleanUp()
{
	bool ret = false;

	PLOG("Destroying all objects");

	for (int i = 0; i < Objects.size(); i++)
		DestroyObject(i);

	Objects.clear();

	// detach log streamW
	aiDetachAllLogStreams();

	return ret;
}

bool ModuleManageMesh::LoadFBX(const char* file_path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_Fast);// for better looks i guess: aiProcessPreset_TargetRealtime_MaxQuality);
	std::string aux = file_path;

	if (scene == nullptr)
	{
		std::string new_file_path = file_path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = scene_folder + new_file_path;

		scene = aiImportFile(new_file_path.c_str(), aiProcessPreset_TargetRealtime_Fast);
		aux = new_file_path;
	}

	obj_data add_obj;
	
	add_obj.name = aux.substr(aux.find_last_of("\\/") + 1);

	if (scene != nullptr && scene->HasMeshes())
	{

		float vertex_aux = 0.f;

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			mesh_data add;
      
			add.num_vertex = scene->mMeshes[i]->mNumVertices;
			add.vertex = new float[add.num_vertex*3];
			add.num_faces = scene->mMeshes[i]->mNumFaces;

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

			App->ui->geo_properties_win->CheckMeshInfo();
		}


		if (Objects.size() > 0)
		{
			DestroyObject(0);
			Objects.insert(Objects.begin(), add_obj);
			Objects.pop_back();
		}
		else
			Objects.push_back(add_obj);

		App->camera->Transport(vec3(vertex_aux + 3, vertex_aux + 3, vertex_aux + 3));
		App->camera->LookAt(vec3(0.0f, 0.0f, 0.0f));
		App->camera->mesh_multiplier = vertex_aux / 4;

		// ReSet all Parenting for later use
		SetParents();

		// Texture Setup
		std::vector<obj_data>::iterator item = --Objects.end();
		if (scene->HasMaterials())
		{
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				SetupMat(*item._Ptr, scene->mMaterials[i]);
				aiString path;
				aiReturn err = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
				LoadTextCurrentObj(path.C_Str(), item._Ptr);
			}
		}

		aiReleaseImport(scene);
	}
	else
	{
		PLOG("Error loading scene's meshes %s", file_path);
		ret = false;
	}

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

		glColor4f(1, 1, 1, 1);

		if (use_texture)
		{
			if(mesh->parent->textures.size() > 0 && mesh)
			int test = mesh->parent->textures.size() - 1;
			if(mesh->parent->textures.size() > 0 && mesh->tex_index <= (mesh->parent->textures.size() - 1))
			{ 
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uvs);
				glTexCoordPointer(3, GL_FLOAT, 0, NULL);

				glBindTexture(GL_TEXTURE_2D, mesh->parent->textures[mesh->tex_index].id_tex);
			}
			else if (mesh->parent->mat_colors.size() > 0)
			{
				Color c = mesh->parent->mat_colors[mesh->tex_index];
				glColor4f(c.r, c.g, c.b, c.a);
			}
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

bool ModuleManageMesh::Load(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");

	scene_folder = json_object_dotget_string(json_object(root_value), "manage_mesh.scenes_path");
	tex_folder = json_object_dotget_string(json_object(root_value), "manage_mesh.textures_path");

	ret = true;
	return ret;
}

bool ModuleManageMesh::Save(JSON_Value * root_value)
{
	bool ret = false;


	//root_value = json_parse_file("config_data.json");
	//JSON_Object* root_obj = json_value_get_object(root_value);
	//
	//
	//
	//json_serialize_to_file(root_value, "config_data.json");

	ret = true;
	return ret;
}

//-SET OBJ DATA------------------------------------------------------------------------------------------

void ModuleManageMesh::SetupMat(obj_data& obj, aiMaterial* material)
{

	aiColor3D color;
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	obj.mat_colors.push_back(Color(color.r,color.g,color.b,1));
	
}

bool ModuleManageMesh::LoadTextCurrentObj(const char* path, obj_data* curr_obj)
{
	bool ret = true;

	curr_obj->textures.push_back(texture_data());
	texture_data* item = (--curr_obj->textures.end())._Ptr;

	if (strrchr(path, '\\') != nullptr)
	{
		item->filename = strrchr(path, '\\');
		item->filename.substr(item->filename.find_first_of("\\")+3);
	}
	else
		item->filename = path;

	// Load Tex parameters and data to vram?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &item->id_tex);
	glBindTexture(GL_TEXTURE_2D, item->id_tex);

	GenTexParams();

	ILuint id_Image;
	ilGenImages(1, &id_Image);
	ilBindImage(id_Image);

	bool check = ilLoadImage(path);

	if (!check)
	{
		std::string new_file_path = path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = tex_folder + new_file_path;

		check = ilLoadImage(new_file_path.c_str());
	}

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
		glDeleteTextures(1, &item->id_tex);
		ret = false;
	}

	

	ilDeleteImages(1, &id_Image);

	glBindTexture(GL_TEXTURE_2D, 0);

	App->ui->geo_properties_win->CheckMeshInfo();

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
		//glDeleteBuffers(GL_ARRAY_BUFFER, &Objects[index].meshes[i].id_normal);
		glDeleteBuffers(1, &Objects[index].meshes[i].id_uvs);
		glDeleteBuffers(1, &Objects[index].meshes[i].id_vertex);

		delete Objects[index].meshes[i].index;
		delete Objects[index].meshes[i].normal;
		delete Objects[index].meshes[i].tex_coords;
		delete Objects[index].meshes[i].vertex;

		if (Objects[index].mathbody != nullptr) {
			Objects[index].mathbody->DelMathBody();
			delete Objects[index].mathbody;
		}
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

void ModuleManageMesh::GenTexParams()
{
	uint tws, twt, tmagf, tminf;

	switch (curr_tws) {
	case (TP_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): tws = GL_CLAMP_TO_EDGE;	break;
	case (TP_CLAMP_TO_BORDER - TP_TEXTURE_WRAP - 1): tws = GL_CLAMP_TO_BORDER;	break;
	case (TP_MIRRORED_REPEAT - TP_TEXTURE_WRAP - 1): tws = GL_MIRRORED_REPEAT; break;
	case (TP_REPEAT - TP_TEXTURE_WRAP - 1):	tws = GL_REPEAT; break;
	case (TP_MIRROR_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): tws = GL_MIRROR_CLAMP_TO_EDGE; break;
	default: PLOG("Unsuccessful initialization");
	}

	switch (curr_twt) {
	case (TP_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): twt = GL_CLAMP_TO_EDGE; break;
	case (TP_CLAMP_TO_BORDER - TP_TEXTURE_WRAP - 1): twt = GL_CLAMP_TO_BORDER; break;
	case (TP_MIRRORED_REPEAT - TP_TEXTURE_WRAP - 1): twt = GL_MIRRORED_REPEAT; break;
	case (TP_REPEAT - TP_TEXTURE_WRAP - 1): twt = GL_REPEAT; break;
	case (TP_MIRROR_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): twt = GL_MIRROR_CLAMP_TO_EDGE; break;
	default: PLOG("Unsuccessful initialization");
	}

	// Texture Filter
	switch (curr_tmagf) {
	case (TP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR; break;
	case (TP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST; break;
	case (TP_NEAREST_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST_MIPMAP_NEAREST; break;
	case (TP_LINEAR_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR_MIPMAP_NEAREST; break;
	case (TP_NEAREST_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST_MIPMAP_LINEAR; break;
	case (TP_LINEAR_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR_MIPMAP_LINEAR; break;
	default: PLOG("Unsuccessful initialization");
	}

	switch (curr_tminf) {
	case (TP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR; break;
	case (TP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST; break;
	case (TP_NEAREST_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST_MIPMAP_NEAREST; break;
	case (TP_LINEAR_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR_MIPMAP_NEAREST; break;
	case (TP_NEAREST_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST_MIPMAP_LINEAR; break;
	case (TP_LINEAR_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR_MIPMAP_LINEAR; break;
	default: PLOG("Unsuccessful initialization");
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tws);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, twt);

	// Texture Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tmagf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tminf);
}

// CREATE PRIMITIVE OBJECTS -------------------------------------------------------------------------------

bool ModuleManageMesh::CreatePrimitiveObject(const vec& center, PCube& cube)
{
	bool ret = true;

	obj_data add_obj;

	PCube* new_cube = new PCube(cube);

	new_cube->MathBody = new AABB(Sphere(center, cube.size.x/2.0f));

	mesh_data mcube;
	mcube.num_vertex = new_cube->MathBody->NumVertices();
	mcube.vertex = new float[mcube.num_vertex*3];
	memcpy(mcube.vertex, &new_cube->MathBody->CornerPoint(0), 3 * sizeof(float)*mcube.num_vertex);

	std::vector<int> vert_order =
	{	0,3,2,	0,1,3,
		1,5,7,	1,7,3,
		5,4,7,	4,6,7,
		4,0,2,	4,2,6,
		2,3,7,	2,7,6,
		0,4,5,	0,5,1
	};

	mcube.num_index = new_cube->MathBody->NumFaces() * 3;
	mcube.index = new GLuint[mcube.num_index];
	memcpy(mcube.index, &vert_order[0], 3 * sizeof(GLuint)*mcube.num_vertex);

	mcube.num_normal = mcube.num_index * 2;
	mcube.normal = new float[mcube.num_normal];

	for (int i = 0; i < mcube.num_index / 3; i++)
		SetNormals(mcube, i);

	std::vector<float> tex_coords =
	{
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	0,1,0,
		1,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	1,0,0,	1,1,0,
		0,0,0,	1,1,0,	0,1,0,
		0,0,0,	0,1,0,	1,1,0,
		0,0,0,	0,1,0,	1,1,0

	};

	mcube.num_uvs = tex_coords.size();
	mcube.tex_coords = new float[mcube.num_uvs];
	memcpy(mcube.tex_coords, &tex_coords[0], sizeof(float)*mcube.num_uvs);

	GenBuffers(mcube);

	add_obj.meshes.push_back(mcube);

	if (Objects.size() > 0)
	{
		DestroyObject(0);
		Objects.insert(Objects.begin(), add_obj);
		Objects.pop_back();
	}
	else
		Objects.push_back(add_obj);

	SetParents();

	return ret;
}