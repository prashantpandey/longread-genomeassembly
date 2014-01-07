/*
 * Cell.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: prashant
 */

#include <iostream>

using namespace std;

#include "Cell.h"

namespace sw {

Cell::Cell() {
	Cell::cost = 0;
	Cell::parent = 0;
}

Cell::Cell(int cost, int parent) {
	Cell::cost = cost;
	Cell::parent = parent;
}

Cell::~Cell() {
	// TODO Auto-generated destructor stub
}

int Cell::getCost() {
	return Cell::cost;
}

int Cell::getParent() {
	return Cell::parent;
}

void Cell::setCost(int cost) {
	Cell::cost = cost;
}

void Cell::setParent(int parent) {
	Cell::parent = parent;
}

void Cell::toString() {
	cout << cost << " " << parent << endl;
}

} /* namespace sw */
