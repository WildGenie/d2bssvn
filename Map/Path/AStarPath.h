#pragma once

#include "Path.h"
#include "..\Map.h"
#include "Reduction\PathReducer.h"

#include <vector>
#include <queue>
#include <cmath>

namespace Mapping
{
namespace Pathing
{

inline int __fastcall Manhattan(Point const & start, Point const & end)
{
	return 10*(std::abs(start.first-end.first) + std::abs(start.second-end.second));
}
inline int __fastcall DiagonalShortcut(Point const & start, Point const & end)
{
	int xdist = std::abs(start.first-end.first);
	int ydist = std::abs(start.second-end.second);
	return ((xdist > ydist) ? 14*ydist + 10*(xdist-ydist) : 14*xdist + 10*(ydist-xdist));
}
inline int __fastcall Chebyshev(Point const & start, Point const & end)
{
	int xdist = (start.first-end.first);
	int ydist = (start.second-end.second);
	return (xdist > ydist ? xdist : ydist);
}

#pragma warning ( disable: 4512 )

class Node
{
public:
	Node* const parent;
	Point point;
	int g, h;

	Node() : parent(0) {}
	Node(Point s, Node* p, int _g, int _h) : point(s), parent(p), g(_g), h(_h) {}
};
struct NodeComparer {
	inline bool __fastcall operator() (const Node* left, const Node* right) const { return (left->g + left->h) > (right->g + right->h); }
};

template <class Allocator = std::allocator<Node> >
class AStarPath : public Path
{
private:
	Allocator alloc;

	Map& map;
	Reducing::PathReducer* reducer;
	Estimator estimate;
	Distance distance;

	inline void ReverseList(Node* node, PointList& list)
	{
		list.clear();
		Node* current = node;
		do { list.insert(list.begin(), current->point); } while((current = current->parent) != NULL);
	}

	void FindPath(Point const & start, Point const & end, Node** result, std::vector<Node*>& nodes, bool abs)
	{
		std::priority_queue<Node*, std::vector<Node*>, NodeComparer> open;
		std::vector<Point> closed;

		Node* begin = alloc.allocate(1);
		// if we don't get a valid node, just return
		if(!begin) return;

		alloc.construct(begin, Node(start, NULL, 0, estimate(map, start, end)));
		nodes.push_back(begin);
		open.push(begin);

		while(!open.empty())
		{
			Node* current = open.top();
			open.pop();

			std::vector<Point>::iterator lbegin = closed.begin(), lend = closed.end();
			if(std::find(lbegin, lend, current->point) != lend) continue;

			if(current->point == end)
			{
				*result = current;
				return;
			}

			closed.push_back(current->point);

			for(int i = 1; i >= -1; i--)
			{
				for(int j = 1; j >= -1; j--)
				{
					if(i == 0 && j == 0) continue;

					Point point(current->point.first + i, current->point.second + j);
					if(!map.IsValidPoint(point, abs) || reducer->Reject(point, abs)) continue;

					Node* next = alloc.allocate(1);
					// if we don't get a valid node, just return
					if(!next) return;
					alloc.construct(next, Node(point, current, current->g + distance(current->point, point),
												estimate(map, point, end)));
					nodes.push_back(next);
					open.push(next);
				}
			}
		}
	}

public:
	AStarPath(Map& _map, Reducing::PathReducer* _reducer, Estimator _estimate, Distance _distance = DiagonalShortcut) :
		map(_map), reducer(_reducer), estimate(_estimate), distance(_distance), alloc(Allocator()) {}

	inline Allocator const & GetAllocator() { return alloc; }
	inline void SetMap(Map& map) { this->map = map; }
	inline void SetPathReducer(Reducing::PathReducer* reducer) { this->reducer = reducer; }

	void GetPath(Point const & start, Point const & end, PointList& list, bool abs = true)
	{
		list.clear();
		Node* result = NULL;

		// if we don't have a valid start and end, don't even bother
		if(!map.IsValidPoint(start, abs) || !map.IsValidPoint(end, abs) ||
			reducer->Reject(start, abs) || reducer->Reject(end, abs)) return;

		std::vector<Node*> nodes;
		FindPath(start, end, &result, nodes, abs);
		if(result)
		{
			PointList in;
			ReverseList(result, in);
			reducer->Reduce(in, list, abs);
		}
		else
			list = PointList();

		std::vector<Node*>::iterator lbegin = nodes.begin(), lend = nodes.end();
		for(std::vector<Node*>::iterator it = lbegin; it != lend; it++)
		{
			alloc.destroy((*it));
			alloc.deallocate((*it), sizeof(*it));
		}
	}
};

#pragma warning ( default: 4512 )

}
}
