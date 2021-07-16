#include "pch.h"
#include "MapGrid.h"

MapCell::MapCell()
	: blockCount(0), visited(false)
{
}

void MapGrid::ClearVisited()
{
	for (int i = 0; i < map.size(); i++)
	{
		for (int j = 0; j < map[i].size(); j++)
		{
			map[i][j].visited = false;
		}
	}
}

MapGrid::MapGrid(int minx, int miny, int maxx, int maxy)
	:xMin(minx), xMax(maxx), yMin(miny), yMax(maxy)
{
	generation = 0;
	width = xMax - xMin + 1;
	height = yMax - yMin + 1;
	map.resize(height, std::vector<MapCell>(width));
}


void MapGrid::ToGridCoord(float x, float y, float& gx, float& gy)
{
	gx = max(min(x, xMax), xMin) - xMin;
	gy = max(min(y, yMax), yMin) - yMin;
}

void MapGrid::ToWorldCoord(float gx, float gy, float& x, float& y)
{
	x = gx + xMin;
	y = gx + yMin;
}

void MapGrid::AddUnit(float x, float y, float size)
{
	float x1, x2, y1, y2;
	ToGridCoord(x - size, y - size, x1, y1);
	ToGridCoord(x + size, y + size, x2, y2);
	int left = (int)floor(x1);
	int right = (int)ceil(x2);
	int bottom = (int)floor(y1);
	int top = (int)ceil(y2);
	for (int i = left; i <= right; i++)
	{
		for (int j = bottom; j <= top; j++)
		{
			map[i][j].blockCount++;
		}
	}
}

void MapGrid::RemoveUnit(float x, float y, float size)
{
	float x1, x2, y1, y2;
	ToGridCoord(x - size, y - size, x1, y1);
	ToGridCoord(x + size, y + size, x2, y2);
	int left = (int)floor(x1);
	int right = (int)ceil(x2);
	int bottom = (int)floor(y1);
	int top = (int)ceil(y2);
	for (int i = left; i <= right; i++)
	{
		for (int j = bottom; j <= top; j++)
		{
			map[i][j].blockCount--;
		}
	}
}

bool MapGrid::IsFree(float x, float y, float size)
{
	float x1, x2, y1, y2;
	ToGridCoord(x - size, y - size, x1, y1);
	ToGridCoord(x + size, y + size, x2, y2);
	int left = (int)floor(x1);
	int right = (int)ceil(x2);
	int bottom = (int)floor(y1);
	int top = (int)ceil(y2);
	for (int i = left; i <= right; i++)
	{
		for (int j = bottom; j <= top; j++)
		{
			if (map[i][j].blockCount > 0) return false;
		}
	}
	return true;
}

float dist(float x1, float x2, float y1, float y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int dx[4] = { -1, 0, 1, 0 };
int dy[4] = { 0, -1, 0, 1 };

Path MapGrid::GetPathToPoint(float startX, float startY, float size, float endX, float endY)
{
	generation++;

	float sxf, syf, exf, eyf;
	ToGridCoord(startX, startY, sxf, syf);
	ToGridCoord(endX, endY, exf, eyf);
	int sx = (int)sxf;
	int sy = (int)syf;
	int ex = (int)exf;
	int ey = (int)eyf;
	RemoveUnit(startX, startY, size);
	//ClearVisited();

	std::priority_queue<SearchNode> searchNodes;
	std::vector<std::unique_ptr<SearchCell>> cells;
	SearchNode start(sx, sy, 0, dist(sx, sy, ex, ey));
	searchNodes.push(start);
	map[sx][sy].visited = generation;

	while (!searchNodes.empty())
	{
		SearchNode current = searchNodes.top();
		if (current.x == ex && current.y == ey)
		{
			Path p;
			std::stack<SearchCell*> stack;
			SearchCell* cur = current.cell;
			while (cur != nullptr)
			{
				stack.push(cur);
				cur = cur->prev;
			}
			while (!stack.empty())
			{
				cur = stack.top();
				float gx = cur->x;
				float gy = cur->y;
				float wx, wy;
				ToWorldCoord(gx, gy, wx, wy);
				p.nodes.push_back(PathNode(wx, wy));
			}
			AddUnit(startX, startY, size);
			return p;
		}
		searchNodes.pop();
		for (int i = 0; i < 4; i++)
		{
			int nx = current.x + dx[i];
			int ny = current.y + dy[i];
			if (map[nx][ny].visited < generation)
			{
				map[nx][ny].visited = generation;
				if (IsFree(nx, ny, size))
				{
					SearchCell* next = new SearchCell();
					next->x = nx;
					next->y = ny;
					next->prev = current.cell;
					cells.emplace_back(next);
					SearchNode nextNode(nx, ny, current.cost + 1, dist(nx, ny, ex, ey));
					nextNode.cell = next;
					searchNodes.push(nextNode);
				}
			}
		}
	}

	AddUnit(startX, startY, size);
	return Path();
}

PathNode::PathNode(float x_, float y_) :
	x(x_), y(y_)
{
}

SearchNode::SearchNode()
{
}

SearchNode::SearchNode(int x_, int y_, float cost_, float heuristic_):
	x(x_), y(y_), cost(cost_), heuristic(heuristic_), cell(nullptr)
{

}

bool SearchNode::operator<(const SearchNode& rhs) const
{
	// can change to A*, heuristic is dist(target, (x,y))
	return cost + heuristic < rhs.cost + rhs.heuristic;
}