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
			
			// For now only push first bone matrix
			bone->transform.SetFromMatrix(&get_matrix);

			ret->bones.push_back(bone);
		}
		ret->OrderBones();

		ret->CalculateSkeletonTransforms();
	}
	else
	{
		delete ret;
		ret = nullptr;
	}

	read_file.close();	

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
	uint bone_size = id_size + sizeof(uint);

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
		uint num_weights = 0;
		uint name_size = 1;
		DGUID id;
		std::string name = "";
		if (it->second->Bone != nullptr)
		{
			num_weights = it->second->Bone->mNumWeights;

			name = it->second->Bone->mName.C_Str();
			name_size += name.length();

			id.cpyfromstring(StringMD5ID(name));
		}
		else
		{
			name = it->second->BoneNode->mName.C_Str();
			name_size += name.length();

			id.cpyfromstring(StringMD5ID(name));
		}

		std::string affected_mesh = ""; 
		uint mesh_id = INT_MAX;
		uint mesh_name_size = 1;
		if (it->second->Mesh != nullptr)
		{
			affected_mesh = GetFileName(path) + "_Mesh_";

			for (int j = 0; j < scene->mNumMeshes; j++)
				if (scene->mMeshes[j] == it->second->Mesh)
				{
					mesh_id = j;
					break;
				}
			affected_mesh += std::to_string(mesh_id) + ".meshdrnk";

			mesh_name_size += affected_mesh.length();
		}

		// These are the original bone, bone node and the transformations between nodes
		// Between nodes sometimes assimp create $AssimpFbx$ nodes that only explain transformations

		buf_size = bone_size + sizeof(uint) * 3 + name_size + mesh_name_size + weight_size * num_weights + sizeof(uint) +  sizeof(float) * 16;

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

		aiMatrix4x4 cpy = it->second->BoneNode->mTransformation;
		if (it->second->AddedTransform.size() > 0)
			cpy = it->second->AddedTransform[it->second->AddedTransform.size() - 1];
		for (int k = it->second->AddedTransform.size() - 2; k >= 0; k--)
			cpy *= it->second->AddedTransform[k];

		if(cpy != it->second->BoneNode->mTransformation)
			cpy *= it->second->BoneNode->mTransformation;
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


std::vector<std::multimap<uint, BoneCrumb*>> SkeletonImport::ReconstructSkeletons(const aiScene* scene, std::vector<const aiNode*>& BoneNodes)
{
	std::vector<std::multimap<uint, BoneCrumb*>> Skeletons;


		for (int i = 0; i < BoneNodes.size(); i++)
		{
			std::multimap<uint, BoneCrumb*> skel;

			for (int j = 0; j < BoneNodes[i]->mNumMeshes; j++)
			{
				aiMesh* bone_mesh = scene->mMeshes[BoneNodes[i]->mMeshes[j]];

				for (int k = 0; k < bone_mesh->mNumBones; k++)
				{
					BoneCrumb* crumb = new BoneCrumb();
					crumb->BoneNode = scene->mRootNode->FindNode(bone_mesh->mBones[k]->mName);
					if (crumb->BoneNode != nullptr)
					{
						uint par_num = 0;
						const aiNode* curr = crumb->BoneNode;
						while (curr != scene->mRootNode) {
							if (std::string(curr->mName.C_Str()).find("$AssimpFbx$") == std::string::npos)
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

			// Set Par_ids for load
			std::multimap<uint, BoneCrumb*>::iterator it;
			for (it = skel.begin(); it != skel.end(); it++)
			{
				std::multimap<uint, BoneCrumb*>::iterator low = skel.upper_bound(it->first - 1);
				aiNode* nonAssimpFbx_par = it->second->BoneNode->mParent;
				while (std::string(nonAssimpFbx_par->mName.C_Str()).find("$AssimpFbx$") != std::string::npos)
				{
					it->second->AddedTransform.push_back(nonAssimpFbx_par->mTransformation);
					nonAssimpFbx_par = nonAssimpFbx_par->mParent;
				}

				while (true && low != skel.end())
				{
					if (low->second->BoneNode == nonAssimpFbx_par)
					{
						it->second->fast_par_id = low->second->fast_id;
						break;
					}

					if (low == skel.begin())
						break;
					else
						low--;
				}
				if (it->second->fast_par_id == 0)
				{
					it->second->fast_par_id = skel.size() + 1;
					BoneCrumb* push = new BoneCrumb(nonAssimpFbx_par);
					push->fast_id = skel.size() + 1;
					skel.insert(std::pair<uint, BoneCrumb*>(it->first - 1, push));
				}
			}
			Skeletons.push_back(skel);
		}		

	return Skeletons;
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