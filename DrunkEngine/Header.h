#ifndef _GAMEOBJECT_
#define _GAMEOBJECT_

#include <vector>
#include <string>
#include "Primitive.h"

class GameObject : public Module
{
public: 



public:
	std::string name;

	std::vector<mesh_data> meshes;

	std::vector<Color> mat_colors;
	std::vector<texture_data> textures;

	aiVector3D transform_position;
	aiVector3D transform_scale;
	aiQuaternion transform_rotate;

	AABB BoundingBox;

	Primitive* mathbody = nullptr; // In case we create a premade object // Temporary solution
};

#endif