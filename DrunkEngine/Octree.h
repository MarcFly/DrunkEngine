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
		Node(std::vector<GameObject*> objs_in_node, Node * parent, Octree * root);
		~Node();

		void Update();
		void Draw();
		void CleanUp();

		void SetNodeVertex();
		void CreateNodes();

	public:
		int id;

		Octree * root = nullptr;
		Node * parent = nullptr;
		std::vector<Node*> child;

		Axis axis_to_check = Axis::Axis_NULL;

		std::vector<GameObject*> objects_in_node;
		std::vector<vec> node_vertex;	//	[0]  X  Y  Z	("-" sign indicates lower value)
										//	[1]  X  Y -Z
										//	[2] -X  Y -Z
										//	[3] -X  Y  Z
										//	[4]  X -Y  Z
										//	[5]  X -Y -Z
										//	[6] -X -Y -Z
										//	[7] -X -Y  Z

	};

public:
	Octree(int elements_per_node);
	~Octree();

	void Update();
	void CleanUp();

	void RecursiveGetStaticObjs(const GameObject * obj);

public:
	int elements_per_node;

	Node * base_node = nullptr;
	std::vector<Node*> nodes;
	std::vector<GameObject*> static_objs;
};


#endif