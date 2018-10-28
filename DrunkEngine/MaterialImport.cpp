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

ComponentMaterial * MatImport::ImportMat(const char* file, GameObject* par, const char* Dir)
{
	App->importer->Imp_Timer.Start();

	ComponentMaterial* ret = new ComponentMaterial();
	ret->parent = par;

	ret->name = file;

	// Default Material Color
	std::ifstream read_file;
	read_file.open(file, std::ios::binary);

	std::streampos end = read_file.seekg(0, read_file.end).tellg();
	read_file.seekg(0, read_file.beg);

	if (end > 0)
	{
		char* data = new char[end];
		read_file.read(data, sizeof(char)*end);
		char* cursor = data;

		float color[4];
		memcpy(&color[0], cursor, sizeof(color));
		ret->default_print.Set(color[0], color[1], color[2], color[3]);
		cursor += sizeof(color);

		memcpy(&ret->NumProperties, cursor, sizeof(uint));
		ret->NumProperties /= sizeof(uint);
		cursor += sizeof(uint);

		memcpy(&ret->NumDiffTextures, cursor, sizeof(uint));
		cursor += sizeof(uint);
		
		uint dir_size;
		memcpy(&dir_size, cursor, sizeof(uint));
		cursor += sizeof(uint);

		char* dir = new char[dir_size+1];
		memcpy(dir, cursor, dir_size+1);
		cursor += dir_size +2;

		std::vector<uint> texture_ranges;
		for (int i = 0; i < ret->NumDiffTextures; i++)
		{
			uint size;
			memcpy(&size, cursor, sizeof(uint));
			texture_ranges.push_back(size);
			cursor += sizeof(uint);
		}


		for (int i = 0; i < ret->NumDiffTextures; i++)
		{
			char* aux = new char[texture_ranges[i]]; // Acounting for exit queues and bit buffer
			memcpy(aux, cursor, texture_ranges[i] * sizeof(char));

 			Texture* check = ImportTexture(aux, ret);

			if (check == nullptr)
			{
				std::string filename = "./Library/Textures/";
				filename += App->importer->GetFileName(aux);
				filename.append(".dds");

				ExportTexture(aux, Dir);
				check = ImportTexture(filename.c_str(), ret, Dir);
			}
			if (check != nullptr)
			{
				ret->textures.push_back(check);
			}
			cursor += texture_ranges[i];
		}

		App->importer->Imp_Timer.LogTime("Texture");

		App->ui->console_win->AddLog("New Material with %d textures loaded.", ret->textures.size());

	}
	else
	{
		delete ret;
		ret = nullptr;
	}

	App->importer->Imp_Timer.LogTime("Material Import");

	return ret;
}

Texture* MatImport::ImportTexture(const char * path, ComponentMaterial* par, const char* Dir)
{
	Timer text_timer;

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

	text_timer.LogTime("Repetition Check");
	text_timer.Start();

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
		
		if (!check) // Check from imported textures folder
		{
			std::string new_file_path = path;
			new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

			new_file_path = "./Library/Textures/" + new_file_path; // Have to set new directories

			check = ilLoadImage(new_file_path.c_str());
			if (check)
				App->ui->console_win->AddLog("Texture found in Imported Directories");
		}

		text_timer.LogTime("Texture Load");
		text_timer.Start();

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

			text_timer.LogTime("Tex full Load");
			text_timer.Start();

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

		text_timer.LogTime("Tex finish Load");
		text_timer.Start();
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

void MatImport::ExportMat(const aiScene * scene, const int& mat_id, const char * path)
{
	aiMaterial* mat = scene->mMaterials[mat_id];

	uint prop_size = mat->mNumProperties;
	uint text_size = mat->GetTextureCount(aiTextureType_DIFFUSE);
	
	uint buf_size = sizeof(uint) * (prop_size + text_size);

	aiColor3D getc;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, getc);
	float color[4] = {getc.r,getc.g,getc.b,1};

	buf_size += sizeof(color);

	// allocating a uint for size
	buf_size += sizeof(uint);
	std::string dir = App->importer->GetDir(path);
	buf_size += dir.length(); // \0

	std::vector<uint> texture_ranges;
	std::vector<std::string> textures; // Only Diffuse for now
	for (int i = 0; i < text_size; i++)
	{
		aiString path;
		mat->GetTexture(aiTextureType_DIFFUSE, i, &path);
		
		textures.push_back(path.C_Str());

		buf_size += textures[i].length() + 2; // It takes the . as an exit queue automatically? also if no \0 it breaks
		
		texture_ranges.push_back(textures[i].length() + 2);
	}

	char* data = new char[buf_size];
	char* cursor = data;

	memcpy(cursor, &color[0], sizeof(color));
	cursor += sizeof(color);
	
	// I will memcpy the properties amount because its relevant
	// but not the properties itself because we don't use them now
	memcpy(cursor, &prop_size, sizeof(uint));
	cursor += sizeof(uint);

	memcpy(cursor, &text_size, sizeof(uint));
	cursor += sizeof(uint);

	uint dir_size = dir.length() + 2;
	memcpy(cursor, &dir_size, sizeof(uint));
	cursor += sizeof(uint);

	memcpy(cursor, dir.c_str(), dir.length());
	cursor += dir.length() + 2;

	char* exitqueue = "\0";
	memcpy(cursor, exitqueue, 2);
	cursor += 2;

	memcpy(cursor, &texture_ranges[0], sizeof(uint)*texture_ranges.size());
	cursor += sizeof(uint) * texture_ranges.size();

	for (int i = 0; i < textures.size(); i++)
	{
		memcpy(cursor, textures[i].c_str(), textures[i].length());
		cursor += textures[i].length();
		memcpy(cursor, exitqueue, 2);
		cursor += 2;
	}

	std::ofstream write_file;
	std::string filename = "./Library/Materials/";
	filename += App->importer->GetFileName(path) + "_Mat_" + std::to_string(mat_id);
	filename.append(".matdrnk");

	write_file.open(filename.c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();
	
}

void MatImport::ExportTexture(const char * path, const char* full_path)
{

	ILuint id_Image;
	ilGenImages(1, &id_Image);
	ilBindImage(id_Image);

	bool check = ilLoadImage(path);

	if (!check) // Check from obj directory
	{
		std::string new_file_path = path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = App->importer->GetDir(full_path) + new_file_path;

		check = ilLoadImage(new_file_path.c_str());

		if (check)
		{
			App->ui->console_win->AddLog("Texture found in Parent Directory: Exporting Texture for next time!");
		}

	}

	if (check)
	{
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

////////////------------------------------------------------------------------------------------------------------------------
//-SaveDT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MatImport::ExportMat(const ComponentMaterial* mat)
{
	uint prop_size = mat->NumProperties;
	uint text_size = mat->NumDiffTextures;

	uint buf_size = sizeof(uint) * (prop_size + text_size);

	float color[4] = { mat->default_print.r,mat->default_print.g,mat->default_print.b,1 };

	buf_size += sizeof(color);

	// allocating a uint for size
	buf_size += sizeof(uint);
	std::string dir = "./Library/Textures/";
	buf_size += dir.length();

	std::vector<uint> texture_ranges;
	std::vector<std::string> textures; // Only Diffuse for now
	for (int i = 0; i < text_size; i++)
	{
		if (mat->textures[i] != nullptr)
		{
			textures.push_back(mat->textures[i]->filename.c_str());

			buf_size += textures[i].length() + 2; // It takes the . as an exit queue automatically? also if no \0 it breaks

			texture_ranges.push_back(textures[i].length() + 2);
		}
	}

	char* data = new char[buf_size];
	char* cursor = data;

	memcpy(cursor, &color[0], sizeof(color));
	cursor += sizeof(color);

	// I will memcpy the properties amount because its relevant
	// but not the properties itself because we don't use them now
	memcpy(cursor, &prop_size, sizeof(uint));
	cursor += sizeof(uint);

	memcpy(cursor, &text_size, sizeof(uint));
	cursor += sizeof(uint);

	uint dir_size = dir.length() + 2;
	memcpy(cursor, &dir_size, sizeof(uint));
	cursor += sizeof(uint);

	memcpy(cursor, dir.c_str(), dir.length());
	cursor += dir.length();

	char* exitqueue = "\0";
	memcpy(cursor, exitqueue, 2);
	cursor += 2;

	memcpy(cursor, &texture_ranges[0], sizeof(uint)*texture_ranges.size());
	cursor += sizeof(uint) * texture_ranges.size();

	for (int i = 0; i < textures.size(); i++)
	{
		memcpy(cursor, textures[i].c_str(), textures[i].length());
		cursor += textures[i].length();
		memcpy(cursor, exitqueue, 2);
		cursor += 2;
	}

	std::ofstream write_file;

	write_file.open(mat->name.c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();

}