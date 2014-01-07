/*
 * Read.h
 *
 * Object containing the read and various parameters related to the read.
 *  Created on: Nov 9, 2013
 *      Author: prashant
 */

#ifndef READ_H_
#define READ_H_

#include <string>

using namespace std;

class Read {
public:
	Read();
	virtual ~Read();

	int id;
	string seq;
	long length;
	long start;
	long end;

	void setId(int id);
	void setSeq(string seq);
	void setLength(long);
	void setStart(long);
	void setEnd(long);

	int getId();
	string getSeq();
	long getLength();
	long getStart();
	long getEnd();
	string getReverseComplement();

	void toString();
};

#endif /* READ_H_ */
