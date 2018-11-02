#include "Octree.h"
#include "Application.h"

Octree::Octree(int elements_per_node)
{
	static_objs.clear();
	nodes.clear();

	this->elements_per_node = elements_per_node;

	RecursiveGetStaticObjs(App->mesh_loader->Root_Object);

	base_node = new Node(static_objs, nullptr, this);
	nodes.push_back(base_node);
}

Octree::~Octree()
{
}

void Octree::Update()
{
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->Update();
}

void Octree::CleanUp()
{
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->CleanUp();

	static_objs.clear();
	nodes.clear();
}

void Octree::RecursiveGetStaticObjs(const GameObject * obj)
{
	for (int i = 0; i < obj->children.size(); i++)
	{
		if (obj->children[i]->is_static)
		{
			static_objs.push_back(obj->children[i]);
			RecursiveGetStaticObjs(obj->children[i]);
		}
	}
}

Octree::Node::Node(std::vector<GameObject*> objs_in_node, Node * parent, Octree * root)
{
	this->root = root;
	this->parent = parent;
	id = root->nodes.size();

	this->objects_in_node = objs_in_node;

	if (parent == nullptr)
		axis_to_check = Axis::Axis_X;

	else if (parent->axis_to_check == Axis::Axis_X)
		axis_to_check = Axis::Axis_Y;

	else if (parent->axis_to_check == Axis::Axis_Y)
		axis_to_check = Axis::Axis_Z;

	else if (parent->axis_to_check == Axis::Axis_Z)
		axis_to_check = Axis::Axis_X;

	SetNodeVertex();

	if (root->elements_per_node < objects_in_node.size())
		CreateNodes();
}

Octree::Node::~Node()
{
}

void Octree::Node::Update()
{
	Draw();
}

void Octree::Node::Draw()
{
	//Draw node AABB
}

void Octree::Node::CleanUp()
{
	root = nullptr;
	parent = nullptr;
	child.clear();
	node_vertex.clear();
	objects_in_node.clear();
}

void Octree::Node::SetNodeVertex()
{
	//Set vertex positions for this node

	vec object_center = objects_in_node[0]->getObjectCenter();

	for (int i = 0; i < 6; i++)
		node_vertex.push_back(object_center);

	for (int i = 1; i < objects_in_node.size(); i++)
	{
		object_center = objects_in_node[i]->getObjectCenter();
		SetVertexPos(object_center);
	}
}

void Octree::Node::CreateNodes()
{
	//Check if there is more than x Gameobjects in this node
	


	//Make two nodes out of the first one



	//Add new nodes to the list
}

void Octree::Node::SetVertexPos(const vec object_center)
{
	//	[0]  X  Y  Z
	if (node_vertex[0].x < object_center.x)
		node_vertex[0].x = object_center.x;

	if (node_vertex[0].y < object_center.y)
		node_vertex[0].y = object_center.y;

	if (node_vertex[0].z < object_center.z)
		node_vertex[0].z = object_center.z;

	//	[1]  X  Y -Z
	if (node_vertex[1].x < object_center.x)
		node_vertex[1].x = object_center.x;

	if (node_vertex[1].y < object_center.y)
		node_vertex[1].y = object_center.y;

	if (node_vertex[1].z > object_center.z)
		node_vertex[1].z = object_center.z;

	//	[2] -X  Y -Z
	if (node_vertex[2].x > object_center.x)
		node_vertex[2].x = object_center.x;

	if (node_vertex[2].y < object_center.y)
		node_vertex[2].y = object_center.y;

	if (node_vertex[2].z > object_center.z)
		node_vertex[2].z = object_center.z;

	//	[3] -X  Y  Z
	if (node_vertex[3].x > object_center.x)
		node_vertex[3].x = object_center.x;

	if (node_vertex[3].y < object_center.y)
		node_vertex[3].y = object_center.y;

	if (node_vertex[3].z < object_center.z)
		node_vertex[3].z = object_center.z;

	//	[4]  X -Y  Z
	if (node_vertex[4].x < object_center.x)
		node_vertex[4].x = object_center.x;

	if (node_vertex[4].y > object_center.y)
		node_vertex[4].y = object_center.y;

	if (node_vertex[4].z < object_center.z)
		node_vertex[4].z = object_center.z;

	//	[5]  X -Y -Z
	if (node_vertex[5].x < object_center.x)
		node_vertex[5].x = object_center.x;

	if (node_vertex[5].y > object_center.y)
		node_vertex[5].y = object_center.y;

	if (node_vertex[5].z > object_center.z)
		node_vertex[5].z = object_center.z;

	//	[6] -X -Y -Z
	if (node_vertex[6].x > object_center.x)
		node_vertex[6].x = object_center.x;

	if (node_vertex[6].y > object_center.y)
		node_vertex[6].y = object_center.y;

	if (node_vertex[6].z > object_center.z)
		node_vertex[6].z = object_center.z;

	//	[7] -X -Y  Z
	if (node_vertex[7].x > object_center.x)
		node_vertex[7].x = object_center.x;

	if (node_vertex[7].y > object_center.y)
		node_vertex[7].y = object_center.y;

	if (node_vertex[7].z < object_center.z)
		node_vertex[7].z = object_center.z;
}
