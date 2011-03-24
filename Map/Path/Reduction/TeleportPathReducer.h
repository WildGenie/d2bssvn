#pragma once

#include "PathReducer.h"
#include "../Path.h"
#include "../../D2Map.h"

namespace Mapping
{
namespace Pathing
{
namespace Reducing
{

#pragma warning ( disable: 4512 )

class TeleportPathReducer : public PathReducer
{
private:
	D2Map* map;
	Distance distance;
	int range;

public:
	TeleportPathReducer(const TeleportPathReducer&);
	TeleportPathReducer& operator=(const TeleportPathReducer&);
	TeleportPathReducer(D2Map* m, Distance d, int _range = 20) : map(m), distance(d), range(_range*10) {}

	void Reduce(PointList const & in, PointList& out, bool abs)
	{
		PointList::const_iterator it = in.begin(), end = in.end();
		out.push_back(*it);
		while(it != end)
		{
			Point prev = *(out.end()-1);
			while(it != end && distance(*it, prev) < range) it++;
			it--;
			out.push_back(*it);
			it++;
		}
	}
	bool Reject(Point const & pt, bool abs)
	{
		return !map->IsValidPoint(pt, abs) ||
			   map->SpaceHasFlag(D2Map::Avoid, pt, abs) ||
			   map->SpaceHasFlag(D2Map::BlockWalk, pt, abs) ||
			   map->SpaceHasFlag(D2Map::BlockPlayer, pt, abs);
	}
	void MutatePoint(Point & pt, bool abs)
	{
		// find the nearest walkable space
		if(Reject(pt, abs)) {
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if(i == 0 && j == 0)
						continue;
					Point ptN(pt.first+i, pt.second+j);
					if(!Reject(ptN, abs)) {
						pt.first = ptN.first;
						pt.second = ptN.second;
					}
				}
			}
		}
	}
};

#pragma warning ( default: 4512 )

}
}
}
