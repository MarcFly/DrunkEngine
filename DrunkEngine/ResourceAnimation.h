#ifndef _Resource_Animation_
#define _Resource_Animation_

#include "Resource.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "AnimationImport.h"
#include "Color.h"
#include "ComponentAnimation.h"

struct Bone;

struct QuatKey {
	double time;
	Quat value;	
};

struct float3Key {
	double time;
	float3 value;
};

struct AnimChannel{
	std::string bone_name = "UnknownBone";
	Bone* curr_bone = nullptr;
	float3Key* TranslationKeys = nullptr;
	uint num_translation_keys = 0;

	QuatKey* RotationKeys = nullptr;
	uint num_rotation_keys = 0;

	float3Key* ScalingKeys = nullptr;
	uint num_scaling_keys = 0;

	void UnloadMem();

	float3Key* LastTKey(float time, float duration, float tickrate);
	QuatKey* LastRKey(float time, float duration, float tickrate);
	float3Key* LastSKey(float time, float duration, float tickrate);
	float3Key* NextTKey(float time, float duration, float tickrate);
	QuatKey* NextRKey(float time, float duration, float tickrate);
	float3Key* NextSKey(float time, float duration, float tickrate);

	float4x4 GetMatrix(float time, float duration, float tickrate);

	AnimChannel() {};
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