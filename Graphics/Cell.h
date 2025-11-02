#pragma once
#include "Definition.h"

class Cell
{

private:
	Point point;
	double g; // cost from start
	double h; // heuristic cost
	double f; // sum of h + f
	Cell* parent;
public:
	Cell(Point cPoint, double gVal, double hVal, Cell* p);

	// getters
	Point getPoint() { return point; }
	double getG() const { return g; }
	double getH() const  { return h; }
	double getF() const { return f; }
	Cell* getParent() { return parent; }

	// setters
	void setH(double newH) { h = newH; }
	void setF(double newF) { f = newF; }
	void setG(double newG) { g = newG; }
	void setParent(Cell* newParent) { parent = newParent; }

	 /* 
	 we added this operator in IPathFinding.cpp
	 bool operator()(const Cell* a, const Cell* b) { return a->f > b->f; }
	 */
};

