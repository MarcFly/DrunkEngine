#include "ComponentMaterial.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"

ComponentMaterial::ComponentMaterial(aiMaterial * mat, GameObject * par)
{
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
		Texture* diff = new Texture;
		LoadTexture(path.C_Str(), diff);

		if (diff != nullptr)
			textures.push_back(diff);
	}

	App->ui->console_win->AddLog("New Material with %d textures loaded.", this->textures.size());
}

void ComponentMaterial::LoadTexture(const char * path, Texture * tex)
{
	if (strrchr(path, '\\') != nullptr)
	{
		tex->filename = strrchr(path, '\\');
		tex->filename.substr(tex->filename.find_first_of("\\") + 3);
	}
	else
		tex->filename = path;

	bool check_rep = false;

	for (int i = 0; i < textures.size(); i++)
	{
		check_rep = (tex->filename.substr(tex->filename.find_last_of("\\/") + 1) == textures[i]->filename);

		if (check_rep)
			break;

		check_rep = (tex->filename.substr(tex->filename.find_last_of("\\/") + 1) == textures[i]->filename.substr(textures[i]->filename.find_last_of("\\/") + 1));

		if (check_rep)
			break;

		check_rep = (tex->filename == textures[i]->filename);

		if (check_rep)
			break;

		check_rep = (tex->filename == textures[i]->filename.substr(textures[i]->filename.find_last_of("\\/") + 1));
	}

	if (!check_rep)
	{
		// Load Tex parameters and data to vram?
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &tex->id_tex);
		glBindTexture(GL_TEXTURE_2D, tex->id_tex);

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

			tex->width = Info.Width;
			tex->height = Info.Height;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

			App->ui->console_win->AddLog("Loaded Texture from path %s, with size %d x %d", path, tex->width, tex->height);

		}
		else
		{
			App->ui->console_win->AddLog("Failed to load image from path %s", path);
			
			glDeleteTextures(1, &tex->id_tex);
			delete tex;
		}

		ilDeleteImages(1, &id_Image);

		glBindTexture(GL_TEXTURE_2D, 0);

		App->ui->geo_properties_win->CheckMeshInfo();
	}
	else
	{

		App->ui->console_win->AddLog("This texture is already loaded!");
		delete tex;
		tex = nullptr;
	}

}

void ComponentMaterial::DestroyTexture(const int& tex_ind)
{
	glDeleteTextures(1, &textures[tex_ind]->id_tex);

	delete textures[tex_ind];

	for (int i = tex_ind + 1; i < textures.size(); i++)
		textures[i - 1] = textures[i];
	
	textures.pop_back();


	if (textures.size() < 1)
		for (int i = 0; i < parent->meshes.size(); i++)
			parent->meshes[i]->Material_Ind = 0;
	else
		for (int i = 0; i < parent->meshes.size(); i++)
			if (parent->meshes[i]->Material_Ind >= textures.size())
				parent->meshes[i]->Material_Ind = - 1;
}

void ComponentMaterial::CleanUp()
{
	for (int i = 0; i < textures.size(); i++)
		DestroyTexture(i);


	parent = nullptr;
}
