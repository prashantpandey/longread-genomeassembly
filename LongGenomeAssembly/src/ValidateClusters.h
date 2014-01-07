/*
 * ValidateClusters.h
 *
 * Test bed to validate the corectness of the hashing/clustering algorithm
 *  Created on: Nov 17, 2013
 *      Author: prashant
 */

#ifndef VALIDATECLUSTERS_H_
#define VALIDATECLUSTERS_H_

#include "Read.h"
#include "Cluster.h"
#include "Smithwaterman.h"

using namespace std;
using namespace lcr;
using namespace sw;

namespace ctb {

class ValidateClusters {
public:
	ValidateClusters();
	ValidateClusters(Cluster*, Read*, long, long, int);
	virtual ~ValidateClusters();

	Cluster* clusters;
	Read* reads;
	long numReads;
	long numClusters;
	int kmer;
	int falsePos;
	int falseNeg;
	int truePos;
	int trueNeg;
	int** multiKmerMatrix;

	int** prepareOverlapMatrix();

	int** getMultiKmerMatrix();
	void validate();

};

} /* namespace ctb */

#endif /* VALIDATECLUSTERS_H_ */
