#include "ComponentMaterial.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"
#include "GameObject.h"
#include "ComponentMesh.h"

ComponentMaterial::ComponentMaterial(aiMaterial * mat, GameObject * par)
{
	this->parent = par;

	// Default Material Color
	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	default_print = { color.r, color.g, color.b, 1 };

	// For future property things
	this->NumProperties = mat->mNumProperties;
	this->NumDiffTextures = mat->GetTextureCount(aiTextureType_DIFFUSE);

	for (int i = 0; i < this->NumDiffTextures; i++)
	{
		aiString path;
		aiReturn err = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		LoadTexture(path.C_Str());
	}

	App->ui->console_win->AddLog("New Material with %d textures loaded.", this->textures.size());
}

void ComponentMaterial::LoadTexture(const char * path)
{
	Texture* ret = new Texture;
	ret->mparent = this;

	bool check_rep = false;

	if (strrchr(path, '\\') != nullptr)
	{
		ret->filename = strrchr(path, '\\');
		ret->filename.substr(ret->filename.find_first_of("\\") + 3);
	}
	else
		ret->filename = path;

	Texture* test = CheckTexRep(ret->filename.c_str());

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
		if(ret->mparent != this)
			ret->referenced_mats.push_back(this);
	}

	if(ret != nullptr)
		this->textures.push_back(ret);
}

void ComponentMaterial::DestroyTexture(const int& tex_ind)
{
	glDeleteTextures(1, &textures[tex_ind]->id_tex);

	if (!textures[tex_ind]->deleted)
	{
		for(int i = 0; i < textures[tex_ind]->referenced_mats.size(); i++)
		{ 
			for (int j = 0; j < textures[tex_ind]->referenced_mats[i]->textures.size(); j++)
			{ 
				if (textures[tex_ind]->referenced_mats[i]->textures[j]->filename == textures[tex_ind]->filename)
				{
					textures[tex_ind]->referenced_mats[i]->textures[j]->deleted = true;
			
				}
			}
				
		}
			

		delete textures[tex_ind];
	}

	textures[tex_ind] = nullptr;

	for (int i = tex_ind + 1; i < textures.size(); i++)
		textures[i - 1] = textures[i];
	
	textures.pop_back();


	if(textures.size() > 0)
		for (int i = 0; i < parent->meshes.size(); i++)
			if (this->parent->meshes[i]->Material_Ind >= textures.size())
				this->parent->meshes[i]->Material_Ind = - 1;
}

void ComponentMaterial::CleanUp()
{
	for (int i = 0; i < textures.size(); i++)
		DestroyTexture(i);


	parent = nullptr;
}

Texture* ComponentMaterial::CheckTexRep(std::string name)
{
	Texture* ret = nullptr;

	GameObject* par = this->parent;

	while (par != NULL && ret == nullptr)
	{
		for (int i = 0; i < par->materials.size(); i++)
		{
			ret = par->materials[i]->CheckNameRep(name);
		}

		par = par->parent;
	}

	return ret;
}

Texture* ComponentMaterial::CheckNameRep(std::string name)
{
	Texture* ret = nullptr;

	for (int i = 0; i < this->textures.size(); i++)
	{
		if(name.substr(name.find_last_of("\\/") + 1) == textures[i]->filename)
			ret = textures[i];

		if (ret != nullptr)
			break;

		if(name.substr(name.find_last_of("\\/") + 1) == textures[i]->filename.substr(textures[i]->filename.find_last_of("\\/") + 1))
		ret = textures[i];
		if (ret != nullptr)
			break;

		if(name == textures[i]->filename)
			ret = textures[i];

		if (ret != nullptr)
			break;

		if(name == textures[i]->filename.substr(textures[i]->filename.find_last_of("\\/") + 1))
			ret = textures[i];
	}

	return ret;
}