#ifndef _FileHelpers_
#define _FileHelpers_

#include <string>
#include "pcg-c-basic-0.9/pcg_basic.h"
#include "SDL/include/SDL_stdinc.h"

std::string GetDir(const char* full_path);

void CreateHiddenDir(const char* dir);

unsigned int GetExtSize(const char* file);

std::string GetExtension(const char* file);

std::string GetFileName(const char* file);

Uint32 GetUUID();

std::string GetMD5ID(const char* file);
std::string GetMD5ID(std::string file);

std::string StringMD5ID(const char* str_pass);
std::string StringMD5ID(std::string str_pass);

enum FileType
{
	FT_Error = -1,
	FT_Texture,
	FT_Object,
	FT_FBX,
	FT_Material,
	FT_Mesh,
	FT_Bilboad,
	FT_Skeleton,

	FT_Files_Max
};

FileType CheckExtension(std::string& ext);

#endif