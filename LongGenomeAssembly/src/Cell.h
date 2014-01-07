/*
 * Cell.h
 *
 * Represents a cell object in a SmithWaterman (dynamic programming) cost matrix
 *  Created on: Nov 14, 2013
 *      Author: prashant
 */

#ifndef CELL_H_
#define CELL_H_

namespace sw {

class Cell {
public:
	Cell();
	Cell(int, int);
	virtual ~Cell();

	int cost;
	int parent;

	int getCost();
	int getParent();

	void setCost(int);
	void setParent(int);

	void toString();
};

} /* namespace sw */

#endif /* CELL_H_ */
