/*
 * ReadOverlap.h
 *
 *  Created on: Nov 9, 2013
 *      Author: prashant
 */

#ifndef READOVERLAP_H_
#define READOVERLAP_H_

#include <string>
using namespace std;

namespace ro {

class ReadOverlap {
public:
	ReadOverlap();
	virtual ~ReadOverlap();

	long getOverlapLen();

	void setOverlapLen(long overlapLen);

	long getReadLen1();

	void setReadLen1(long startRead1);

	long getReadLen2();

	void setReadLen2(long endRead2);

	long overlapLen;
	long readLen1;
	long readLen2;

	void toString();


};

} /* namespace ro */

#endif /* READOVERLAP_H_ */
