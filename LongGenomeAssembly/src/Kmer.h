/*
 * Kmer.h
 *
 *  Created on: Dec 1, 2013
 *      Author: prashant
 */

#ifndef KMER_H_
#define KMER_H_

#include <iostream>

using namespace std;

namespace lcr {

class Kmer {
public:
	Kmer();
	Kmer(int, string);
	virtual ~Kmer();

	int id;
	string kmer;

	int getId();
	string getKmer();
	void setId(int);
	void setKmer(string);
	int compare(Kmer);
};

} /* namespace lcr */

#endif /* KMER_H_ */
