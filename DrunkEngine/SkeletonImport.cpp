#include "SkeletonImport.h"
#include "GameObject.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "InspectorWindow.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "ResourceSkeleton.h"
#include "Resource.h"
#include "ComponentSkeleton.h"

#include <fstream>
#include <iostream>
#include <experimental/filesystem>

void SkeletonImport::Init()
{

}

void SkeletonImport::LinkSkeleton(DGUID fID, ComponentSkeleton* skel)
{
	MetaSkeleton* res = (MetaSkeleton*)App->resources->Library.at(fID);

	skel->name = res->file;

	if (!res->Asset.IsLoaded())
		res->Asset.LoadToMem();

	skel->r_skel = res->Asset.skeleton.ptr;
	skel->UID = fID;
	res->UseCount++;
}

ResourceSkeleton* SkeletonImport::LoadSkeleton(const char* file)
{
	ResourceSkeleton* ret = new ResourceSkeleton();

	std::ifstream read_file;
	read_file.open(file, std::fstream::in | std::fstream::binary);

	int size = read_file.seekg(0, read_file.end).tellg();
	read_file.seekg(0, read_file.beg);

	if (size > 0)
	{
		char* data = new char[size];
		read_file.read(data, sizeof(char)*size);
		char* cursor = data;

		uint num_bones;
		memcpy(&num_bones, cursor, sizeof(uint));
		cursor += sizeof(uint);

		for (int i = 0; i < num_bones; i++)
		{
			Bone* bone = new Bone();

			memcpy(&bone->fast_id, cursor, sizeof(uint));
			cursor += sizeof(uint);
			memcpy(&bone->fast_par_id, cursor, sizeof(uint));
			cursor += sizeof(uint);

			char id[32];
			memcpy(id, cursor, 32);
			cursor += 32;
			bone->ID.cpyfromstring(std::string(id, 32));

			uint num_weights;
			memcpy(&num_weights, cursor, sizeof(uint));
			cursor += sizeof(uint);

			uint name_size;
			memcpy(&name_size, cursor, sizeof(uint));
			cursor += sizeof(uint);

			char* cpy_name = new char[name_size];
			memcpy(cpy_name, cursor, name_size);
			cursor += name_size;
			bone->name.clear();
			bone->name = std::string(cpy_name, name_size);

			uint mesh_name_size;
			memcpy(&mesh_name_size, cursor, sizeof(uint));
			cursor += sizeof(uint);

			char* cpy_mesh_name = new char[mesh_name_size];
			memcpy(cpy_mesh_name, cursor, mesh_name_size);
			cursor += mesh_name_size;
			bone->affected_mesh.clear();
			bone->affected_mesh = std::string(cpy_mesh_name, mesh_name_size);

			for (int j = 0; j < num_weights; j++)
			{
				BoneWeight* weight = new BoneWeight();
				memcpy(&weight->VertexID, cursor, sizeof(uint));
				cursor += sizeof(uint);

				memcpy(&weight->w, cursor, sizeof(float));
				cursor += sizeof(float);

				bone->weights.push_back(weight);
			}

			aiMatrix4x4 get_matrix;
			memcpy(&get_matrix, cursor, sizeof(float) * 16);
			cursor += sizeof(float) * 16;
			
			bone->transform.SetFromMatrix(&get_matrix);

			ret->bones.push_back(bone);
		}
	}
	else
	{
		delete ret;
		ret = nullptr;
	}

	read_file.close();

	ret->OrderBones();

	return ret;
}

void SkeletonImport::ExportMapSkeleton(const aiScene* scene, const aiNode* SkelRoot, std::multimap<uint, BoneCrumb*>& Skeleton, const int& skel_id, const aiNode* root, const char* path)
{
	std::string filename = ".\\Library\\";
	filename += GetFileName(path) + "_Skel_" + std::to_string(skel_id);

	std::fstream write_file;

	write_file.open((filename + ".skeldrnk").c_str(), std::fstream::out | std::fstream::binary);

	uint buf_size = 0;
	uint skeleton_size = Skeleton.size();
	uint weight_size = sizeof(float) + sizeof(uint);
	uint id_size = 32;
	uint bone_size = id_size + sizeof(uint) + sizeof(float) * 16;

	char* data = new char[sizeof(uint)];
	memcpy(data, &skeleton_size, sizeof(uint));
	write_file.write(data, sizeof(uint));

	delete data;
	data = nullptr;
	std::multimap<uint, BoneCrumb*>::iterator it = Skeleton.begin();

	for (int i = 0; i < skeleton_size; i++, it++)
	{
		write_file.close();
		write_file.open((filename + ".skeldrnk").c_str(), std::fstream::app | std::fstream::binary);
		
		uint buf_size = 0;
		uint num_weights = it->second->Bone->mNumWeights;

		std::string name = it->second->Bone->mName.C_Str();
		uint name_size = name.length() + 1;

		DGUID id;
		id.cpyfromstring(StringMD5ID(name));

		std::string affected_mesh = GetFileName(path) + "_Mesh_";
		uint mesh_id;
		for (int j = 0; j < scene->mNumMeshes; j++)
			if (scene->mMeshes[j] == it->second->Mesh)
			{
				mesh_id = j; 
				break;
			}
		affected_mesh += std::to_string(mesh_id) + ".meshdrnk";

		uint mesh_name_size = affected_mesh.length() + 1;

		buf_size = bone_size + sizeof(uint) * 4 + name_size + mesh_name_size + weight_size * num_weights;

		data = new char[buf_size];
		char* cursor = data;

		// Bone Info Copy
		memcpy(cursor, &it->second->fast_id, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, &it->second->fast_par_id, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, id.MD5ID, id_size);
		cursor += id_size;
		memcpy(cursor, &num_weights, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, &name_size, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, name.c_str(), name.length());
		cursor += name.length();
		memcpy(cursor, "\0", 1);
		cursor += 1;
		memcpy(cursor, &mesh_name_size, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, affected_mesh.c_str(), affected_mesh.length());
		cursor += affected_mesh.length();
		memcpy(cursor, "\0", 1);
		cursor += 1;

		// Weights Copy
		for (int j = 0; j < num_weights; j++)
		{
			memcpy(cursor, &it->second->Bone->mWeights[j].mVertexId, sizeof(uint));
			cursor += sizeof(uint);
			memcpy(cursor, &it->second->Bone->mWeights[j].mWeight, sizeof(float));
			cursor += sizeof(float);
		}

		// OffsetMatrix Copy
		aiMatrix4x4 cpy = it->second->Bone->mOffsetMatrix;
		cpy =  cpy * it->second->BoneNode->mTransformation;

		const aiNode* curr_par = it->second->BoneNode->mParent;
		while (curr_par != SkelRoot)
		{
			cpy = cpy * curr_par->mTransformation;
			curr_par = curr_par->mParent;
		}

		{
			memcpy(cursor, &cpy.a1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.a2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.a3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.a4, sizeof(float));	cursor += sizeof(float);

			memcpy(cursor, &cpy.b1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.b2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.b3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.b4, sizeof(float));	cursor += sizeof(float);

			memcpy(cursor, &cpy.c1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.c2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.c3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.c4, sizeof(float));	cursor += sizeof(float);

			memcpy(cursor, &cpy.d1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.d2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.d3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy.d4, sizeof(float));	cursor += sizeof(float);
		}

		write_file.write(data, buf_size);

		delete data;
		data = nullptr;
	}
	
	write_file.close();

	ExportMeta(filename);
}


void SkeletonImport::ExportMeta(std::string& filename)
{
	std::string meta_name = filename + ".meta";
	JSON_Value* meta_file = json_parse_file(meta_name.c_str());
	meta_file = json_value_init_object();

	JSON_Object* meta_obj = json_value_get_object(meta_file);

	json_object_dotset_string(meta_obj,"File", std::string(filename + ".skeldrnk").c_str());

	json_serialize_to_file(meta_file, meta_name.c_str());

	// Free Meta Value
	json_object_clear(meta_obj);
	json_value_free(meta_file);
}

void SkeletonImport::LoadMeta(const char* file, MetaSkeleton* meta)
{
	meta->type = RT_Skeleton;

	JSON_Value* meta_file = json_parse_file(file);
	JSON_Object* meta_obj = json_value_get_object(meta_file);

	meta->file = json_object_dotget_string(meta_obj, "File");

	// Free Meta Value
	json_object_clear(meta_obj);
	json_value_free(meta_file);
}


// SKELETON RECONSTRUCTION HELPERS------------------------------------------


std::vector<std::multimap<uint, BoneCrumb*>> SkeletonImport::ReconstructSkeletons(const aiScene* scene, std::vector<const aiNode*>& SkeletonRoots, std::vector<const aiNode*>& BoneNodes)
{
	std::vector<std::multimap<uint, BoneCrumb*>> Skeletons;

	for (int l = 0; l < SkeletonRoots.size(); l++)
	{
		std::multimap<uint, BoneCrumb*> skel;

		for (int i = 0; i < BoneNodes.size(); i++)
		{
			for (int j = 0; j < BoneNodes[i]->mNumMeshes; j++)
			{
				aiMesh* bone_mesh = scene->mMeshes[BoneNodes[i]->mMeshes[j]];

				for (int k = 0; k < bone_mesh->mNumBones; k++)
				{
					BoneCrumb* crumb = new BoneCrumb();
					crumb->BoneNode = SkeletonRoots[l]->FindNode(bone_mesh->mBones[k]->mName);
					if (crumb->BoneNode != nullptr)
					{
						uint par_num = 0;
						const aiNode* curr = crumb->BoneNode;
						while (curr != scene->mRootNode) {
							par_num++;
							curr = curr->mParent;
						}
						crumb->Bone = bone_mesh->mBones[k];
						crumb->Mesh = bone_mesh;

						crumb->fast_id = skel.size() + 1;
						skel.insert(std::pair<uint, BoneCrumb*>(par_num, crumb));
					}
					else
						delete crumb;
				}
			}
		}
		
		// Set Par_ids for load
		std::multimap<uint, BoneCrumb*>::iterator it;
		for (it = skel.begin(); it != skel.end(); it++)
		{
			std::multimap<uint, BoneCrumb*>::iterator low = skel.upper_bound(it->first - 1);
			if(low != skel.begin() && low != skel.end())
			{
				const aiNode* curr_par = it->second->BoneNode->mParent;
				do {
					low--;
					std::multimap<uint, BoneCrumb*>::iterator it2 = low;
					do {
						if (curr_par == it2->second->BoneNode)
							it->second->fast_par_id = it2->second->fast_id;

						it2--;
					} while (it2 != skel.begin() && it->second->fast_par_id == 0);

					curr_par = curr_par->mParent;
				} while (low != skel.begin() && it->second->fast_par_id == 0);				
			}
			
		}
		
		Skeletons.push_back(skel);
	}

	return Skeletons;
}

void SkeletonImport::GetSkeletonRoots(std::vector<const aiNode*>& BoneNodes, std::vector<const aiNode*>& SkeletonRoots)
{
 	for (int i = 0; i < BoneNodes.size(); i++)
	{
		if (BoneNodes[i] != nullptr)
		{
			const aiNode* SkelRoot = nullptr;
			const aiNode* curr_par = BoneNodes[i];

			bool par_found = false;
			for (int j = 0; j < SkeletonRoots.size(); j++)
				if (SkeletonRoots[j]->FindNode(curr_par->mName))
					par_found = true;

			while (!par_found && SkelRoot == nullptr && curr_par != nullptr)
			{
				if (curr_par->mNumChildren > 1 || curr_par->mParent == nullptr )
				{
					bool par_swap = false;
					const aiNode* last_par = curr_par;
					for (int j = 0; j < curr_par->mNumChildren; j++)
					{
						last_par = curr_par;
						if (std::string(curr_par->mName.C_Str()).find("$AssimpFbx$", 0) > 0)
						{
							par_swap = true;
							curr_par = curr_par->mParent;
							break;
						}
					}
					if (!par_swap)
						SkelRoot = curr_par;
					if (curr_par == nullptr)
						SkelRoot = last_par;
				}
				else
					curr_par = curr_par->mParent;
			}

			if(SkelRoot != nullptr)
				SkeletonRoots.push_back(SkelRoot);

			
		}
	}
}

void SkeletonImport::FindBoneNodes(aiMesh** meshes, const aiNode* node, std::vector<const aiNode*>& bone_nodes)
{
	for (int j = 0; j < node->mNumMeshes; j++)
	{
		if (meshes[node->mMeshes[j]]->HasBones())
			bone_nodes.push_back(node);
	}
	for (int i = 0; i < node->mNumChildren; i++)
		FindBoneNodes(meshes, node->mChildren[i], bone_nodes);
	
}