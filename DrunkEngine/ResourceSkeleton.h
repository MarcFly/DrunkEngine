#include "SkeletonImport.h"
#include "Resource.h"
#include "Color.h"
#include "ComponentTransform.h"

struct DGUID;

struct BoneWeight
{
	uint VertexID = -1;
	float w = 0;
};

struct Bone
{
	Bone() {};

	DGUID ID;
	std::string name = "Unknown_Bone";
	std::string affected_mesh = "UnknownMesh";
	std::vector<BoneWeight*> weights;
	float4x4 matrix;

	~Bone()
	{
		for(int i = 0; i < weights.size(); i++)
		{
			delete weights[i];
			weights[i] = nullptr;
		}
		weights.clear();
		name.clear();	
	}
};

struct ResourceSkeleton
{
	std::vector<Bone*> bones;

	void UnloadMem();
	~ResourceSkeleton();
};

class MetaSkeleton : public MetaResource
{
public:
	MetaSkeleton() { type = RT_Skeleton; };
	~MetaSkeleton() {};

	DGUID origin_mesh;
	
	void LoadMetaFile(const char* file);
};