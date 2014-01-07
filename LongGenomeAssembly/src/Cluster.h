/*
 * Cluster.h
 * Object to store a cluster as the final output of the program
 *  Created on: Nov 15, 2013
 *      Author: prashant
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <vector>

#include "Kmer.h"

using namespace std;
namespace lcr {

class Cluster {
public:
	Cluster();
	Cluster(int);
	virtual ~Cluster();

	static int kmerId;
	int id;
	vector<int> reads;
	vector<int> kmerIds;
	vector<Kmer> kmers;

	void setId(int);
	int getId();
	vector<int> getReads();
	void addRead(int);
	void removeRead(int);
	void flushReads();
	bool checkRead(int);

	vector<Kmer> getKmers();
	void addKmer(string kmer);
	int checkKmer(string kmer);

	void toString();
};

} /* namespace lcr */

#endif /* CLUSTER_H_ */
