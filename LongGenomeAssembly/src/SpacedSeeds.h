/*
 * SpacedSeeds.h
 *
 *  Created on: Nov 24, 2013
 *      Author: prashant
 */

#ifndef SPACEDSEEDS_H_
#define SPACEDSEEDS_H_

#include "Cluster.h"

using namespace lcr;

namespace ss {

/**
 * 	'S' string of {0,1} of length 'L'
 * 	'Q' seed (K-mer) of length 'k'
 * 	'M_max' Maximum size of 'Q'
 * 	'W' weight of 'Q' i.e number of 1's in Q
 * 	'P' probability for each bit to be 1
 * 	The probability of finding local alignments, under specific conditions is called 'Sensitivity'
 * 	There's a trade-off between 'Sensitivity' and 'Specificity'
 * 	** We have to devise an algorithm to look for 15% error rate for calculating the weights of the seeds**
 * 	The greater the 'W', greater will be 'Sensitivity' but lesser the 'Specificity'
 * 	It has been noticed that doubling the number of seeds can account for
 * 	the decrease in 'W', thus simultaneously increasing Sensitivity without reducing Specificity
 * 	Have to take 'Overlap Complexity' to come up with the optimal seed
 *
 * 	Algorithm:
 * 	For a given k-mer size, generate all the possible seeds using the following parameters:
 * 		> weight: should be 85% of the length of the k-mer
 * 		> Avoid certain distribution of '0', like contiguous and equal intervals
 * 	Run the hashing algorithm for all the seeds
 * 	Dump the results in an output file and compare them to find the optimal spaced seed.
 *
 *
 * 	For now one spaced seed will correspond to specific string of '0' and '1'
 *
 */

class SpacedSeeds {
public:
	SpacedSeeds();
	SpacedSeeds(int[]);
	virtual ~SpacedSeeds();

	int length;
	int weight;
	int probability;
	int* seed;
	Cluster* clusters;

	/**
	 * Have to arrange the all possible clusters in lexicographical order
	 */
	Cluster* getClusters(long base, long modulo);

	void addLetters(string str, int tgt_depth, long base, long modulo);

};

} /* namespace ss */

#endif /* SPACEDSEEDS_H_ */
