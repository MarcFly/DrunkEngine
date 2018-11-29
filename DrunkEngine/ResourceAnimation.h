#ifndef _Resource_Animation_
#define _Resource_Animation_

#include "Resource.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "AnimationImport.h"
#include "Color.h"
#include "ComponentAnimation.h"

struct QuatKey {
	double time;
	Quat value;	
};

struct float3Key {
	double time;
	float3 value;
};

struct AnimChannel{
	std::string bone_name;

	float3Key* TranslationKeys;
	uint num_translation_keys;

	QuatKey* RotationKeys;
	uint num_rotation_keys;

	float3Key* ScalingKeys;
	uint num_scaling_keys;

	void UnloadMem();

	~AnimChannel() {
		UnloadMem();
	}
};

struct ResourceAnimation {
	std::string name = "UnknownLink";
	std::vector<AnimChannel*> channels;
	void UnloadMem();

	~ResourceAnimation() {
		UnloadMem();
	}
};

class MetaAnimation : public MetaResource {
public:
	MetaAnimation() { type = RT_Animation; };
	~MetaAnimation() {};

	DGUID og_skel_id;
	std::string og_skel_str;
	double duration;
	double tickrate;
	void LoadMetaFile(const char* file);
};

#endif