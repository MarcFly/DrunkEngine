#ifndef _FileHelpers_
#define _FileHelpers_

#include <string>
#include "pcg-c-basic-0.9/pcg_basic.h"
#include "SDL/include/SDL_stdinc.h"

std::string GetDir(const char* full_path);

void CreateHiddenDir(const char* dir);

unsigned int GetExtSize(const char* file);

std::string GetFileName(const char* file);

Uint32 GetUUID();

std::string GetMD5ID(const char* file);
std::string GetMD5ID(std::string file);

#endif