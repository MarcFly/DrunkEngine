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

void CallLog(const char* str, char* usrData);

ModuleManageMesh::ModuleManageMesh(bool start_enabled) : Module(start_enabled)
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

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = CallLog;
	aiAttachLogStream(&stream);

	return ret;
}

bool ModuleManageMesh::Start()
{
	bool ret = true;

	Load(nullptr);
	SetCurrParams();
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
	//add_obj.name = scene->mRootNode->mName.C_Str();

	if (scene != nullptr && scene->HasMeshes())
	{

		float vertex_aux = 0.f;

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			mesh_data add;
      
			//if (CANNOT READ MESH) TODO
			//	continue;

			aiQuaternion rotation_quat;
			scene->mRootNode->mChildren[i]->mTransformation.Decompose(add.transform_scale, rotation_quat, add.transform_position);
			add.transform_rotate = rotation_quat.GetEuler();

			add.num_vertex = scene->mMeshes[i]->mNumVertices;
			add.vertex = new float[add.num_vertex*3];
			add.num_faces = scene->mMeshes[i]->mNumFaces;

			add.name = scene->mRootNode->mChildren[i]->mName.C_Str();

			memcpy(add.vertex, scene->mMeshes[i]->mVertices, 3*sizeof(float)*add.num_vertex);

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
						App->ui->console_win->AddLog("WARNING, geometry face with != 3 indices!");
						ret = false; //if we want to stop load
					}
					else
					{
						memcpy(&add.index[j*3], scene->mMeshes[i]->mFaces[j].mIndices, 3*sizeof(GLuint));
					
						SetNormals(add, j);						
					}
				}			

				add.box_x = add.vertex[0];
				add.box_nx = add.vertex[0];
				add.box_y = add.vertex[1];
				add.box_ny = add.vertex[1];
				add.box_z = add.vertex[2];
				add.box_nz = add.vertex[2];

				for (uint j = 0; j < scene->mMeshes[i]->mNumVertices * 3; j++)
				{
					if (vertex_aux < abs(add.vertex[j]))
						vertex_aux = abs(add.vertex[j]);

					if (j % 3 == 0 && add.box_x < add.vertex[j])
						add.box_x = add.vertex[j];

					if (j % 3 == 0 && add.box_nx > add.vertex[j])
						add.box_nx = add.vertex[j];

					if (j % 3 == 1 && add.box_y < add.vertex[j])
						add.box_y = add.vertex[j];

					if (j % 3 == 1 && add.box_ny > add.vertex[j])
						add.box_ny = add.vertex[j];

					if (j % 3 == 2 && add.box_z < add.vertex[j])
						add.box_z = add.vertex[j];

					if (j % 3 == 2 && add.box_nz > add.vertex[j])
						add.box_nz = add.vertex[j];
				}
			}

			SetTexCoords(&add, scene->mMeshes[i]);

			add.tex_index = scene->mMeshes[i]->mMaterialIndex;

			GenBuffers(add);

			App->ui->console_win->AddLog("New mesh with %d vertices, %d indices, %d faces (tris)", add.num_vertex, add.num_index, add.num_faces);

			add_obj.meshes.push_back(add);

			App->ui->geo_properties_win->CheckMeshInfo();
		}
		
		SetObjBoundBox(add_obj, scene);
		
		App->camera->SetToObj(&add_obj, vertex_aux);

		// Texture Setup
		if (scene->HasMaterials())
		{
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				SetupMat(add_obj, scene->mMaterials[i]);
				aiString path;
				aiReturn err = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
				LoadTextCurrentObj(path.C_Str(), &add_obj);
			}
		}

		App->ui->console_win->AddLog("New Object with %d meshes, %d texture/s, %d color/s", add_obj.meshes.size(), add_obj.textures.size(), add_obj.mat_colors.size());

		if (Objects.size() > 0)
		{
			DestroyObject(0);
			Objects.insert(Objects.begin(), add_obj);
			Objects.pop_back();
		}
		else
			Objects.push_back(add_obj);
		
		// ReSet all Parenting for later use
		SetParents();

		

		aiReleaseImport(scene);
	}
	else
	{
		App->ui->console_win->AddLog("Error loading scene's meshes %s", file_path);
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

	curr_tws = json_object_dotget_number(json_object(root_value), "texture.curr_wrap_s");
	curr_twt = json_object_dotget_number(json_object(root_value), "texture.curr_wrap_t");
	curr_tmagf = json_object_dotget_number(json_object(root_value), "texture.curr_min_filter");
	curr_tminf = json_object_dotget_number(json_object(root_value), "texture.curr_mag_filter");

	ret = true;
	return ret;
}

bool ModuleManageMesh::Save(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	JSON_Object* root_obj = json_value_get_object(root_value);

	json_object_dotset_number(root_obj, "texture.curr_wrap_s", curr_tws);
	json_object_dotset_number(root_obj, "texture.curr_wrap_t", curr_twt);
	json_object_dotset_number(root_obj, "texture.curr_min_filter", curr_tmagf);
	json_object_dotset_number(root_obj, "texture.curr_mag_filter", curr_tminf);

	json_serialize_to_file(root_value, "config_data.json");

	App->ui->console_win->AddLog("Texture config saved");

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

	if (curr_obj->textures.size() >= 10)
	{
		App->ui->console_win->AddLog("You are loading quite the amount of textures, the first one loaded will now be freed!");
		DestroyTexture(curr_obj, 0);
	}

	curr_obj->textures.push_back(texture_data());

	texture_data* item = (--curr_obj->textures.end())._Ptr;

	if (strrchr(path, '\\') != nullptr)
	{
		item->filename = strrchr(path, '\\');
		item->filename.substr(item->filename.find_first_of("\\") + 3);
	}
	else
		item->filename = path;

	bool check_rep = false;

	for (int i = 0; i < curr_obj->textures.size() - 1; i++)
	{
		check_rep = (item->filename.substr(item->filename.find_last_of("\\/") + 1) == curr_obj->textures[i].filename);
		
		if (check_rep)
			break;

		check_rep = (item->filename.substr(item->filename.find_last_of("\\/") + 1) == curr_obj->textures[i].filename.substr(curr_obj->textures[i].filename.find_last_of("\\/") + 1));

		if (check_rep)
			break;

		check_rep = (item->filename == curr_obj->textures[i].filename);
		
		if (check_rep)
			break;
		
		check_rep = (item->filename == curr_obj->textures[i].filename.substr(curr_obj->textures[i].filename.find_last_of("\\/") + 1));
	}

	if (!check_rep)
	{
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
			// Basically if the direct load does not work, it will get the name of the file and load it from the texture folder if its there
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
			App->ui->console_win->AddLog("Loaded Texture from path %s, with size %d x %d", path, item->width, item->height);

		}
		else
		{
			App->ui->console_win->AddLog("Failed to load image from path %s", path);
			curr_obj->textures.pop_back();
			glDeleteTextures(1, &item->id_tex);
			ret = false;
		}
	
		ilDeleteImages(1, &id_Image);

		glBindTexture(GL_TEXTURE_2D, 0);

		App->ui->geo_properties_win->CheckMeshInfo();
	}
	else
	{

		App->ui->console_win->AddLog("This texture is already loaded!");
		curr_obj->textures.pop_back();
		ret = false;
	}

	

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
		App->ui->console_win->AddLog("No texture coordinates to be set");

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

vec3 ModuleManageMesh::getObjectCenter(const obj_data* obj)
{

	float aux_x = (obj->box_x + obj->box_nx) / 2;
	float aux_y = (obj->box_y + obj->box_ny) / 2;
	float aux_z = (obj->box_z + obj->box_nz) / 2;

	return vec3(aux_x, aux_y, aux_z);
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

void ModuleManageMesh::DestroyTexture(obj_data* curr_obj, const int& tex_ind)
{
	glDeleteTextures(1, &curr_obj->textures[tex_ind].id_tex);

	for (int i = tex_ind + 1; i < curr_obj->textures.size(); i++)
	{
		curr_obj->textures[i - 1] = curr_obj->textures[i];
	}
	curr_obj->textures.pop_back();


	if (curr_obj->textures.size() < 1)
		for (int i = 0; i < curr_obj->meshes.size(); i++)
			curr_obj->meshes[i].tex_index = 0;
	else
		for (int i = 0; i < curr_obj->meshes.size(); i++)
			if (curr_obj->meshes[i].tex_index >= curr_obj->textures.size())
				curr_obj->meshes[i].tex_index = curr_obj->textures.size() - 1;
			
}

void ModuleManageMesh::GenTexParams()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tws);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, twt);

	// Texture Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tmagf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tminf);
}

void ModuleManageMesh::SetCurrParams()
{
	switch (curr_tws) {
	case (TP_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): tws = GL_CLAMP_TO_EDGE;	break;
	case (TP_CLAMP_TO_BORDER - TP_TEXTURE_WRAP - 1): tws = GL_CLAMP_TO_BORDER;	break;
	case (TP_MIRRORED_REPEAT - TP_TEXTURE_WRAP - 1): tws = GL_MIRRORED_REPEAT; break;
	case (TP_REPEAT - TP_TEXTURE_WRAP - 1):	tws = GL_REPEAT; break;
	case (TP_MIRROR_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): tws = GL_MIRROR_CLAMP_TO_EDGE; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}

	switch (curr_twt) {
	case (TP_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): twt = GL_CLAMP_TO_EDGE; break;
	case (TP_CLAMP_TO_BORDER - TP_TEXTURE_WRAP - 1): twt = GL_CLAMP_TO_BORDER; break;
	case (TP_MIRRORED_REPEAT - TP_TEXTURE_WRAP - 1): twt = GL_MIRRORED_REPEAT; break;
	case (TP_REPEAT - TP_TEXTURE_WRAP - 1): twt = GL_REPEAT; break;
	case (TP_MIRROR_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): twt = GL_MIRROR_CLAMP_TO_EDGE; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}

	// Texture Filter
	switch (curr_tmagf) {
	case (TP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR; break;
	case (TP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST; break;
	case (TP_NEAREST_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST_MIPMAP_NEAREST; break;
	case (TP_LINEAR_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR_MIPMAP_NEAREST; break;
	case (TP_NEAREST_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST_MIPMAP_LINEAR; break;
	case (TP_LINEAR_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR_MIPMAP_LINEAR; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}

	switch (curr_tminf) {
	case (TP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR; break;
	case (TP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST; break;
	case (TP_NEAREST_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST_MIPMAP_NEAREST; break;
	case (TP_LINEAR_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR_MIPMAP_NEAREST; break;
	case (TP_NEAREST_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST_MIPMAP_LINEAR; break;
	case (TP_LINEAR_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR_MIPMAP_LINEAR; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}
}

void ModuleManageMesh::SetCurrTexTo(obj_data& curr_obj, const int tex_ind)
{
	for (int i = 0; i < curr_obj.meshes.size(); i++)
		curr_obj.meshes[i].tex_index = tex_ind;
}

void ModuleManageMesh::SetObjBoundBox(obj_data &object, const aiScene* scene)
{
	object.box_x = object.meshes[0].box_x;
	object.box_nx = object.meshes[0].box_nx;
	object.box_y = object.meshes[0].box_y;
	object.box_ny = object.meshes[0].box_ny;
	object.box_z = object.meshes[0].box_z;
	object.box_nz = object.meshes[0].box_nz;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		if (object.box_x < object.meshes[i].box_x)
			object.box_x = object.meshes[i].box_x;

		if (object.box_nx > object.meshes[i].box_nx)
			object.box_nx = object.meshes[i].box_nx;

		if (object.box_y < object.meshes[i].box_y)
			object.box_y = object.meshes[i].box_y;

		if (object.box_ny > object.meshes[i].box_ny)
			object.box_ny = object.meshes[i].box_ny;

		if (object.box_z < object.meshes[i].box_z)
			object.box_z = object.meshes[i].box_z;

		if (object.box_nz > object.meshes[i].box_nz)
			object.box_nz = object.meshes[i].box_nz;
	}
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

void CallLog(const char* str, char* usrData)
{
	App->ui->console_win->AddLog(str);
}