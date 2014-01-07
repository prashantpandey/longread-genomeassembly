/*
 * OverlapMatrix.h
 *
 *  Created on: Dec 6, 2013
 *      Author: prashant
 */

#ifndef OVERLAPMATRIX_H_
#define OVERLAPMATRIX_H_

#include <iostream>

using namespace std;

namespace sw {

class OverlapMatrix {
public:
	OverlapMatrix();
	virtual ~OverlapMatrix();

	int readId1;
	int readId2;
	char orientation;
	int ahg;
	int bhg;
	int score;

	void setReadId1(int);
	void setReadId2(int);
	void setOrientation(char);
	void setAhg(int);
	void setBhg(int);
	void setScore(int);

	int getReadId1();
	int getReadId2();
	char getOrientation();
	int getAhg();
	int getBhg();
	int getScore();

	string toString();

	OverlapMatrix(int, int, char, int, int, int);
};

} /* namespace sw */

#endif /* OVERLAPMATRIX_H_ */
