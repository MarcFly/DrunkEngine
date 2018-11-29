#ifndef _Resource_Animation_
#define _Resource_Animation_

#include "Resource.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "AnimationImport.h"
#include "Color.h"
#include "ComponentAnimation.h"

struct AnimChannels{
	std::string bone_name;
	std::vector<float3> TranslationKeys;
	std::vector<Quat> RotationKeys;
	std::vector<float3> ScalingKeys;

	~AnimChannels() {
		TranslationKeys.clear();
		RotationKeys.clear();
		ScalingKeys.clear();
		bone_name.clear();
	}
};

struct ResourceAnimation {
	std::string name = "UnknownLink";
	std::vector<AnimChannels*> channels;

	~ResourceAnimation() {
		for (int i = 0; i < channels.size(); i++)
		{
			delete channels[i];
			channels[i] = nullptr;
		}
		channels.clear();
	}
};

class MetaAnimation : public MetaResource {
	MetaAnimation() { type = RT_Animation; };
	~MetaAnimation() {};

	DGUID origin_skeleton;

	void LoadMetaFile(const char* file);
};

#endif