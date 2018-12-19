#include "SkeletonImport.h"
#include "Resource.h"
#include "Color.h"
#include "ComponentTransform.h"

struct DGUID;

struct BoneWeight
{
	BoneWeight() {};
	uint VertexID = -1;
	float w = 0;

	BoneWeight(const BoneWeight* cpy)
	{
		VertexID = cpy->VertexID;
		w = cpy->w;
	}
	void operator=(const BoneWeight& cpy)
	{
		VertexID = cpy.VertexID;
		w = cpy.w;
	}
};

struct Bone
{
	Bone() {};
	Bone(const Bone* cpy);

	bool active = false;
	DGUID ID;
	std::string name = "Unknown_Bone";
	std::string affected_mesh = "UnknownMesh";
	std::vector<BoneWeight*> weights;
	ComponentTransform transform;

	std::vector<Bone*> children;

	uint fast_id = 0;
	uint fast_par_id = 0;
	Bone* parent = nullptr;

	void OrderBones();
	void AdjustChildren(const int& i);
	Bone*  GetChild(const uint& par_id);
	void CalculateBoneGlobalTransforms();
	Bone* FindBone(const std::string &bonename);

	~Bone()
	{
		for(int i = 0; i < weights.size(); i++)
		{
			delete weights[i];
			weights[i] = nullptr;
		}
		weights.clear();

		for (int i = 0; i < children.size(); ++i)
		{
			delete children[i];
			children[i] = nullptr;
		}
		children.clear();

		name.clear();	
	}
};

struct ResourceSkeleton
{
	std::vector<Bone*> bones;

	std::vector<Bone*> AnimSkel;

	void OrderBones();
	void AdjustChildren(const int& i);
	Bone*  GetChild(const uint& par_id);
	Bone* FindBone(const std::string &bonename);
	void CalculateSkeletonTransforms();
	
	void CreateAnimSkel(std::vector<Bone*>& bones_to_cpy, Bone* AnimBone = nullptr);

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