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

Octree::Node::Node(std::vector<GameObject*> possible_objs_in_node, Node * parent, Octree * root)
{
	this->root = root;
	this->parent = parent;
	id = root->nodes.size();

	CreateNode();
}
