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

	axis_to_check = Axis::Axis_X;

	SetNodeVertex();

	if (root->elements_per_node < objects_in_node.size())
		CreateNodes();
}

Octree::Node::Node(std::vector<GameObject*> objs_in_node, Node * parent, std::vector<vec> node_vertex)
{
	this->root = parent->root;
	this->parent = parent;
	id = root->nodes.size();

	this->node_vertex = node_vertex;
	this->objects_in_node = objs_in_node;

	if (parent->axis_to_check == Axis::Axis_X)
		axis_to_check = Axis::Axis_Y;

	else if (parent->axis_to_check == Axis::Axis_Y)
		axis_to_check = Axis::Axis_Z;

	else if (parent->axis_to_check == Axis::Axis_Z)
		axis_to_check = Axis::Axis_X;

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
	switch (axis_to_check)
	{
	case Axis::Axis_X:
	{
		//	 X Y Z / -X Y Z
		vec center_1 = (node_vertex[0] + node_vertex[3]) / 2;

		//	 X Y-Z / -X Y-Z 
		vec center_2 = (node_vertex[1] + node_vertex[2]) / 2;

		//	 X-Y-Z / -X-Y-Z
		vec center_3 = (node_vertex[5] + node_vertex[6]) / 2;

		//	 X-Y Z / -X-Y Z 
		vec center_4 = (node_vertex[4] + node_vertex[7]) / 2;

		std::vector<vec> new_coords_Px = node_vertex;
		std::vector<vec> new_coords_Nx = node_vertex;

		new_coords_Px[3] = center_1;
		new_coords_Px[2] = center_2;
		new_coords_Px[6] = center_3;
		new_coords_Px[7] = center_4;

		new_coords_Nx[0] = center_1;
		new_coords_Nx[1] = center_2;
		new_coords_Nx[5] = center_3;
		new_coords_Nx[4] = center_4;
		
		Node * Px = new Node(GetObjectsInNode(new_coords_Px), this, new_coords_Px);
		Node * Nx = new Node(GetObjectsInNode(new_coords_Nx), this, new_coords_Nx);

		root->nodes.push_back(Px);
		root->nodes.push_back(Nx);

		child.push_back(Px);
		child.push_back(Nx);

		break;
	}
	case Axis::Axis_Y:
	{
		//	 X Y Z /  X-Y Z
		vec center_1 = (node_vertex[0] + node_vertex[4]) / 2;

		//	 X Y-Z /  X-Y-Z 
		vec center_2 = (node_vertex[1] + node_vertex[5]) / 2;

		//	-X Y-Z / -X-Y-Z
		vec center_3 = (node_vertex[2] + node_vertex[6]) / 2;

		//	-X Y Z / -X-Y Z 
		vec center_4 = (node_vertex[3] + node_vertex[7]) / 2;

		std::vector<vec> new_coords_Py = node_vertex;
		std::vector<vec> new_coords_Ny = node_vertex;

		new_coords_Py[4] = center_1;
		new_coords_Py[5] = center_2;
		new_coords_Py[6] = center_3;
		new_coords_Py[7] = center_4;

		new_coords_Ny[0] = center_1;
		new_coords_Ny[1] = center_2;
		new_coords_Ny[2] = center_3;
		new_coords_Ny[3] = center_4;

		Node * Py = new Node(GetObjectsInNode(new_coords_Py), this, new_coords_Py);
		Node * Ny = new Node(GetObjectsInNode(new_coords_Ny), this, new_coords_Ny);

		root->nodes.push_back(Py);
		root->nodes.push_back(Ny);

		child.push_back(Py);
		child.push_back(Ny);

		break;
	}
	case Axis::Axis_Z:
	{
		//	 X Y Z /  X Y-Z
		vec center_1 = (node_vertex[0] + node_vertex[1]) / 2;

		//	-X Y Z / -X Y-Z 
		vec center_2 = (node_vertex[3] + node_vertex[2]) / 2;

		//	 X-Y Z /  X-Y-Z
		vec center_3 = (node_vertex[4] + node_vertex[5]) / 2;

		//	-X-Y Z / -X-Y-Z 
		vec center_4 = (node_vertex[6] + node_vertex[7]) / 2;

		std::vector<vec> new_coords_Pz = node_vertex;
		std::vector<vec> new_coords_Nz = node_vertex;

		new_coords_Pz[1] = center_1;
		new_coords_Pz[2] = center_2;
		new_coords_Pz[5] = center_3;
		new_coords_Pz[6] = center_4;

		new_coords_Nz[0] = center_1;
		new_coords_Nz[3] = center_2;
		new_coords_Nz[4] = center_3;
		new_coords_Nz[7] = center_4;


		Node * Pz = new Node(GetObjectsInNode(new_coords_Pz), this, new_coords_Pz);
		Node * Nz = new Node(GetObjectsInNode(new_coords_Nz), this, new_coords_Nz);

		root->nodes.push_back(Pz);
		root->nodes.push_back(Nz);

		child.push_back(Pz);
		child.push_back(Nz);

		break;
	}
	}
}

std::vector<GameObject*> Octree::Node::GetObjectsInNode(std::vector<vec> new_node_vertex)
{
	std::vector<GameObject*> objs_in_new_node;
	AABB bb_node = AABB(new_node_vertex[6], new_node_vertex[0]);

	for (int i = 0; i < objects_in_node.size(); i++)
	{
		if (bb_node.Contains(objects_in_node[i]->getObjectCenter()))
			objs_in_new_node.push_back(objects_in_node[i]);
	}

	return objs_in_new_node;
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
