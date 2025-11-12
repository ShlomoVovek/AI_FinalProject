#pragma once
#include <cmath>


const double PI = 3.14;
const double SIGHT_RANGE = 25.0;
const int MSX = 80;
const int MSY = 48;

const double COST_UNKNOWN = 10.0;
const double COST_SAFE = 1.0;
const double COST_DANGER = 15.0;

// types of cells in map
enum CellType
{
	EMPTY = 0,
	ROCK = -2, // SAFE: cannot shoot, see, cross 
	WATER = -3,// NOT SAFE: can shoot or see, cannot corss
	TREE = -4, // NOT SAFE: can shoot and see, cannot cross
	BLUE_BASE = -5, // ammo and medicine
	RED_BASE = -6 // ammo and medicine
};

struct Point
{
	int x;
	int y;

	// Add comparison operators for easier debugging
	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const Point& other) const {
		return !(*this == other);
	}
};

inline double Distance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

inline double Distance(Point a, Point b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

inline double EuclideanDist(double dx, double dy)
{
	return sqrt((dx * dx) + (dy * dy));
}

inline int ManhattanDistance(int x1, int y1, int x2, int y2)
{

	return abs(x2 - x1) + abs(y2 - y1);
}

inline double ManhattanDistance(Point a, Point b)
{
	return abs(b.x - a.x) + abs(b.y - b.y);
}
