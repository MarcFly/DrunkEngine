#include "MaterialImport.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "GameObject.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "InspectorWindow.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "FileHelpers.h"

#include <fstream>
#include <iostream>

void MatImport::Init()
{
	// DevIL initialization
	ilInit();
	iluInit();
}

////////////------------------------------------------------------------------------------------------------------------------
//-IMPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MatImport::LinkMat(DGUID fID, ComponentMaterial* mat)
{
	MetaMat* meta = (MetaMat*)App->resources->Library.at(fID);

	if (!meta->Asset.IsLoaded())
		meta->Asset.LoadToMem();

	meta->UseCount++;

	mat->r_mat = meta->Asset.mat.ptr;
}

ResourceTexture* MatImport::LinkTexture(DGUID fID)
{
	MetaTexture* meta = (MetaTexture*)App->resources->Library.at(fID);

	if (!meta->Asset.IsLoaded())
		meta->Asset.LoadToMem();

	meta->UseCount++;

	return meta->Asset.texture.ptr;
}

ResourceMaterial* MatImport::LoadMat(const char* file)
{
	ResourceMaterial* r_mat = new ResourceMaterial();

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
		r_mat->default_print.Set(color[0], color[1], color[2], color[3]);
		cursor += sizeof(color);

		memcpy(&r_mat->NumProperties, cursor, sizeof(uint));
		cursor += sizeof(uint);

		memcpy(&r_mat->NumDiffTextures, cursor, sizeof(uint));
		cursor += sizeof(uint);
		
		uint dir_size;
		memcpy(&dir_size, cursor, sizeof(uint));
		cursor += sizeof(uint);

		char* dir = new char[dir_size+1];
		memcpy(dir, cursor, dir_size+1);
		cursor += dir_size;

		std::vector<uint> texture_ranges;
		for (int i = 0; i < r_mat->NumDiffTextures; i++)
		{
			uint size;
			memcpy(&size, cursor, sizeof(uint));
			texture_ranges.push_back(size);
			cursor += sizeof(uint);
		}


		for (int i = 0; i < r_mat->NumDiffTextures; i++)
		{
			char* aux = new char[texture_ranges[i]]; // Acounting for exit queues and bit buffer
			memcpy(aux, cursor, texture_ranges[i] * sizeof(char));

			std::string filename = ".\\Library\\" + GetFileName(aux);
			DGUID tfID(filename.c_str());
			if (!App->resources->InLibrary(tfID))
			{
				MetaTexture* map_tex = new MetaTexture();
				std::string meta_file = filename + ".meta";
				map_tex->LoadMetaFile(meta_file.c_str());
				tfID = DGUID(map_tex->file.c_str());
				App->resources->Library[tfID] = map_tex;
				
			}		
			r_mat->textures.push_back(tfID);
			cursor += texture_ranges[i];
		}

		App->ui->console_win->AddLog("New Material with %d textures loaded.", r_mat->textures.size());

	}
	else
	{
		delete r_mat;
		r_mat = nullptr;
	}

	return r_mat;

}

ResourceTexture* MatImport::LoadTexture(const char * path)
{
	ResourceTexture* ret = new ResourceTexture();
	
	// Load Tex parameters and data to vram?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &ret->id_tex);
	glBindTexture(GL_TEXTURE_2D, ret->id_tex);

	App->renderer3D->GenTexParams();

	ILuint id_Image;
	ilGenImages(1, &id_Image);
	ilBindImage(id_Image);

	bool check = ilLoadImage(path);

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

	App->ui->inspector_win->CheckMeshInfo();	

	return ret;
}

////////////------------------------------------------------------------------------------------------------------------------
//-EXPORT-//------------------------------------------------------------------------------------------------------------------
////////////------------------------------------------------------------------------------------------------------------------

void MatImport::ExportAIMat(const aiMaterial * mat, const int& mat_id, const char * path)
{
	uint prop_size = mat->mNumProperties;
	uint text_size = mat->GetTextureCount(aiTextureType_DIFFUSE);
	
	uint buf_size = sizeof(uint) * 2;

	aiColor3D getc;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, getc);
	float color[4] = {getc.r, getc.g, getc.b, 1};

	buf_size += sizeof(color);

	// allocating a uint for size
	buf_size += sizeof(uint);
	std::string dir = GetDir(path);
	buf_size += dir.length() + 2; // \0

	std::vector<uint> texture_ranges;
	std::vector<std::string> textures; // Only Diffuse for now
	for (int i = 0; i < text_size; i++)
	{
		aiString aipath;
		mat->GetTexture(aiTextureType_DIFFUSE, i, &aipath);
		
		ExportILTexture(aipath.C_Str(), path);

		textures.push_back(aipath.C_Str());

		buf_size += textures[i].length() + GetExtSize(textures[i].c_str()) + 2; // It takes the . as an exit queue automatically? also if no \0 it breaks
		
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
	cursor += dir.length();

	char* exitqueue = "\0";
	memcpy(cursor, exitqueue, 2);
	cursor += 2;

	if (texture_ranges.size() > 0)
	{
		memcpy(cursor, &texture_ranges[0], sizeof(uint)*texture_ranges.size());
		cursor += sizeof(uint) * texture_ranges.size();

		for (int i = 0; i < textures.size(); i++)
		{
			memcpy(cursor, textures[i].c_str(), textures[i].length() + GetExtSize(textures[i].c_str()));
			cursor += textures[i].length() + GetExtSize(textures[i].c_str());
			memcpy(cursor, exitqueue, 2);
			cursor += 2;
		}
	}

	std::ofstream write_file;
	std::string filename = ".\\Library\\";
	filename += GetFileName(path) + "_Mat_" + std::to_string(mat_id);

	write_file.open((filename + ".matdrnk").c_str(), std::fstream::out | std::ios::binary);

	write_file.write(data, buf_size);

	write_file.close();

	ExportMeta(mat, mat_id, filename);
}

void MatImport::ExportILTexture(const char * path, const char* full_path)
{
	ILuint id_Image;
	ilGenImages(1, &id_Image);
	ilBindImage(id_Image);

	std::string libpath = ".\\Library\\" + GetFileName(path) + ".dds";
	bool check = ilLoadImage(libpath.c_str());

	if (!check) // Check from obj directory
	{
		std::string new_file_path = path;
		new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

		new_file_path = GetDir(full_path) + new_file_path;

		check = ilLoadImage(new_file_path.c_str());

		if (check)
			App->ui->console_win->AddLog("Texture found in Parent Directory: Exporting Texture for next time!");
	}
	
	if (check)
	{
		check = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		std::string export_path = ".\\Library\\";
		export_path.append(GetFileName(path).c_str());

		ilSave(IL_DDS, (export_path + ".dds").c_str());

		ExportMetaTex(export_path);

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
	uint prop_size = mat->r_mat->NumProperties;
	uint text_size = mat->r_mat->NumDiffTextures;

	uint buf_size = sizeof(uint) * 2;

	float color[4] = { mat->r_mat->default_print.r,mat->r_mat->default_print.g,mat->r_mat->default_print.b,1 };

	buf_size += sizeof(color);

	// allocating a uint for size
	buf_size += sizeof(uint);
	std::string dir = ".\\Library\\";
	buf_size += dir.length() + 2;

	std::vector<uint> texture_ranges;
	std::vector<std::string> textures; // Only Diffuse for now
	for (int i = 0; i < text_size; i++)
	{
		MetaTexture* m_tex = (MetaTexture*)App->resources->Library.at(mat->r_mat->textures[i]);
		if ( m_tex != nullptr)
		{
			textures.push_back(m_tex->file.c_str());

			buf_size += textures[i].length() + GetExtSize(textures[i].c_str()) + 2; // It takes the . as an exit queue automatically? also if no \0 it breaks

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

//--------------------------------------

void MatImport::ExportMeta(const aiMaterial* mat, const int& mat_id, std::string& path)
{
	std::string meta_name = path + ".meta";
	JSON_Value* meta_file = json_parse_file(path.c_str());
	meta_file = json_value_init_object();

	JSON_Object* meta_obj = json_value_get_object(meta_file);

	json_object_dotset_string(meta_obj, "File", std::string(path + ".matdrnk").c_str());

	json_serialize_to_file(meta_file, meta_name.c_str());
}
void MatImport::LoadMeta(const char* file, MetaMat* meta)
{
	meta->type = RT_Material;

	JSON_Value* meta_file = json_parse_file(file);
	JSON_Object* meta_obj = json_value_get_object(meta_file);

	meta->file = json_object_dotget_string(meta_obj, "File");
}

void MatImport::ExportMetaTex(std::string path)
{
	std::string meta_name = path + ".meta";
	JSON_Value* meta_file = json_parse_file(meta_name.c_str());
	meta_file = json_value_init_object();

	JSON_Object* meta_obj = json_value_get_object(meta_file);

	json_object_dotset_string(meta_obj, "File", std::string(path + ".dds").c_str());

	json_serialize_to_file(meta_file, meta_name.c_str());
}
void MatImport::LoadMetaTex(const char* file, MetaTexture* meta)
{
	meta->type = RT_Texture;

	JSON_Value* meta_file = json_parse_file(file);
	JSON_Object* meta_obj = json_value_get_object(meta_file);

	meta->file = json_object_dotget_string(meta_obj, "File");
}