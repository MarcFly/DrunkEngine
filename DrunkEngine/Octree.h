#ifndef _OCTREE_H_
#define _OCTREE_H_

#include <vector>
#include "Globals.h"

class GameObject;

class Octree
{
	class Node
	{
	public:
		enum Axis
		{
			Axis_X,
			Axis_Y,
			Axis_Z,
			Axis_NULL
		};

	public:
		Node(std::vector<GameObject*> possible_objs_in_node, Node * parent);
		~Node();

		void Update();
		void Draw();
		void CleanUp();

		void CreateNode();

	public:
		Node * parent = nullptr;
		std::vector<Node*> child;

		Axis axis_to_check = Axis::Axis_NULL;
		std::vector<vec> node_vertex;
	};

public:
	Octree(int elements_per_node);
	~Octree();

	void Update();
	void CleanUp();

	GameObject* RecursiveGetStaticObjs(GameObject * obj) const;

public:
	int elements_per_node;

	std::vector<Node*> nodes;
	std::vector<GameObject*> static_objs;
};


#endif
