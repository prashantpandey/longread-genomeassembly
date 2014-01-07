/*
 * CellIdx.h
 * Gives the idx attached to any cell
 *  Created on: Nov 14, 2013
 *      Author: prashant
 */

#ifndef CELLIDX_H_
#define CELLIDX_H_

#include <iostream>

using namespace std;

namespace sw {

class CellIdx {
public:
	CellIdx() {
		this->row = 0;
		this->column = 0;
	}

	CellIdx(int i, int j) {
		this->row = i;
		this->column = j;
	}
	virtual ~CellIdx() {

	}

	int row;
	int column;

	void toString() {
		cout << row << " " << column << endl;
	}

};

} /* namespace sw */

#endif /* CELLIDX_H_ */
