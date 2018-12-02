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

			for (int j = 0; j < num_weights; j++)
			{
				BoneWeight* weight = new BoneWeight();
				memcpy(&weight->VertexID, cursor, sizeof(uint));
				cursor += sizeof(uint);

				memcpy(&weight->w, cursor, sizeof(float));
				cursor += sizeof(float);

				bone->weights.push_back(weight);
			}

			memcpy(bone->matrix.v, cursor, sizeof(float) * 16);
			cursor += sizeof(float) * 16;

			ret->bones.push_back(bone);
		}
	}
	else
	{
		delete ret;
		ret = nullptr;
	}

	read_file.close();

	return ret;
}

void SkeletonImport::ExportAISkeleton(const aiNode* root, const aiMesh* mesh, const int& mesh_id, const char* path)
{
	
	std::string filename = ".\\Library\\";
	filename += GetFileName(path) + "_Mesh_" + std::to_string(mesh_id) + "_Skel";

	std::fstream write_file;

	write_file.open((filename + ".skeldrnk").c_str(), std::fstream::out | std::fstream::binary);

	uint buf_size = 0;

	uint skeleton_size = mesh->mNumBones;
	uint weight_size = sizeof(float) + sizeof(uint);
	uint id_size = 32;
	uint bone_size = id_size + sizeof(uint) + sizeof(float) * 16;

	char* data = new char[sizeof(uint)];
	memcpy(data, &skeleton_size, sizeof(uint));
	write_file.write(data, sizeof(uint));

	delete data;
	data = nullptr;

	write_file.close();
	write_file.open((filename + ".skeldrnk").c_str(), std::fstream::app | std::fstream::binary);

	for (int i = 0; i < skeleton_size; i++)
	{
		buf_size = 0;

		
		uint num_weights = mesh->mBones[i]->mNumWeights;

		std::string name = mesh->mBones[i]->mName.C_Str();
		uint name_size = name.length() + 1;

		DGUID id;
		id.cpyfromstring(StringMD5ID(name));

		buf_size = bone_size + sizeof(uint) + name_size + weight_size * num_weights;

		data = new char[buf_size];
		char* cursor = data;

		// Bone Info Copy
		memcpy(data, id.MD5ID, id_size);
		cursor += id_size;
		memcpy(cursor, &num_weights, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, &name_size, sizeof(uint));
		cursor += sizeof(uint);
		memcpy(cursor, name.c_str(), name.length());
		cursor += name.length();
		memcpy(cursor, "\0", 1);
		cursor += 1;

		// Weights Copy
		for (int j = 0; j < num_weights; j++)
		{
			memcpy(cursor, &mesh->mBones[i]->mWeights[j].mVertexId, sizeof(uint));
			cursor += sizeof(uint);
			memcpy(cursor, &mesh->mBones[i]->mWeights[j].mWeight, sizeof(float));
			cursor += sizeof(float);
		}

		// OffsetMatrix Copy
		{
			const aiMatrix4x4* cpy = &root->FindNode(mesh->mBones[i]->mName)->mTransformation;

			memcpy(cursor, &cpy->a1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->a2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->a3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->a4, sizeof(float));	cursor += sizeof(float);

			memcpy(cursor, &cpy->b1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->b2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->b3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->b4, sizeof(float));	cursor += sizeof(float);

			memcpy(cursor, &cpy->c1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->c2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->c3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->c4, sizeof(float));	cursor += sizeof(float);

			memcpy(cursor, &cpy->d1, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->d2, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->d3, sizeof(float));	cursor += sizeof(float);
			memcpy(cursor, &cpy->d4, sizeof(float));	cursor += sizeof(float);
		}
		write_file.write(data, buf_size);

		delete data;
		data = nullptr;
	}

	write_file.close();

	ExportMeta(mesh, mesh_id, path);
	
}

void SkeletonImport::ExportMeta(const aiMesh* mesh, const int& mesh_id, std::string path)
{
	std::string meta_name = ".\\Library\\" + GetFileName(path.c_str()) + "_Mesh_" + std::to_string(mesh_id) + "_Skel.meta";
	JSON_Value* meta_file = json_parse_file(path.c_str());
	meta_file = json_value_init_object();

	JSON_Object* meta_obj = json_value_get_object(meta_file);

	std::string write = ".\\Library\\" + GetFileName(path.c_str()) + "_Mesh_" + std::to_string(mesh_id);
	
	json_object_dotset_string(meta_obj,"File", std::string(write + "_Skel.skeldrnk").c_str());
	
	write += ".meshdrnk";
	write = DGUID(write.c_str()).MD5ID;
	write[32] = '\0';
	json_object_dotset_string(meta_obj, "Mesh", write.c_str());

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
	meta->origin_mesh = json_object_dotget_string(meta_obj, "Mesh");

	// Free Meta Value
	json_object_clear(meta_obj);
	json_value_free(meta_file);
}