/*
 * Smithwaterman.h
 *
 *  Created on: Nov 11, 2013
 *      Author: prashant
 */

#ifndef SMITHWATERMAN_H_
#define SMITHWATERMAN_H_

#include <string>

#include "Cell.h"
#include "CellIdx.h"
#include "OverlapMatrix.h"

using namespace std;

namespace sw {

class Smithwaterman {
public:
	Smithwaterman(string, string);
	virtual ~Smithwaterman();

	static const int MATCH = 0;
	static const int INSERT = 1;
	static const int  DELETE = 2;

	string str1;
	string str2;

	Cell** cell;
	CellIdx goalCell;
	CellIdx startCell;

	void matchOut(int, int);
	void insertOut(int);
	void deleteOut(int);

	static void readIpFile(string fileName);
	void reconstructPath(int, int);
	Cell** costMatrixInit(int, int);
	CellIdx getGoalCell(int, int);

	void printMatrix(int, int);
	OverlapMatrix prepareOverlapMatrix(int, int, int, int);

	int calculateAlignment();

};

} /* namespace sw */

#endif /* SMITHWATERMAN_H_ */
