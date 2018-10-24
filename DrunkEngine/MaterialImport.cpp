#include "MaterialImport.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "GameObject.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

void MatImport::Init()
{
	// DevIL initialization
	ilInit();
	iluInit();
}

////////////------------------------------------------------------------------------------------------------------------------
//-IMPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

ComponentMaterial * MatImport::ImportMaterial(const aiMaterial * mat, GameObject* par)
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
		Texture* add = ImportTexture(path.C_Str(), ret);
		if (add != nullptr)
			ret->textures.push_back(add);
	}

	App->ui->console_win->AddLog("New Material with %d textures loaded.", ret->textures.size());

	return ret;
}

Texture* MatImport::ImportTexture(const char * path, ComponentMaterial* par)
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

////////////------------------------------------------------------------------------------------------------------------------
//-EXPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MatImport::ExportTexture(const char * path)
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
			export_path.append(App->importer->GetFileName(path).c_str());
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