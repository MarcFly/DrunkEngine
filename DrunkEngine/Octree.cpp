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


}

void Octree::Node::CreateNodes()
{
	//Check if there is more than x Gameobjects in this node
	


	//Make two nodes out of the first one



	//Add new nodes to the list
}
