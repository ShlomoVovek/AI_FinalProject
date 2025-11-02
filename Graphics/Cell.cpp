#include "Cell.h"


Cell::Cell(Point cPoint, double gVal, double hVal, Cell* p)
{
	point = cPoint;
	g = gVal;
	h = hVal;
	f = h + g;
	parent = p;
}
