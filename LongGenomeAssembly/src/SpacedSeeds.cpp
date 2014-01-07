/*
 * SpacedSeeds.cpp
 *
 *  Created on: Nov 24, 2013
 *      Author: prashant
 */

#include <iostream>
#include <sstream>
#include <string>

#include "SpacedSeeds.h"
#include "Hashing.h"

using namespace std;

namespace ss {

SpacedSeeds::SpacedSeeds() {
	SpacedSeeds::length = 0;
	SpacedSeeds::probability = 0;
	SpacedSeeds::weight = 0;
	SpacedSeeds::seed = NULL;
	SpacedSeeds::clusters = NULL;
}

SpacedSeeds::SpacedSeeds(int* seed) {
	SpacedSeeds::length = 0;
	SpacedSeeds::probability = 0;
	SpacedSeeds::weight = 0;
	SpacedSeeds::seed = seed;
	SpacedSeeds::clusters = new Cluster[1299827];
}

SpacedSeeds::~SpacedSeeds() {
	// TODO Auto-generated destructor stub
}

char letters[] = { 'A', 'C', 'G', 'T' };

Cluster* SpacedSeeds::getClusters(long base, long modulo) {
	SpacedSeeds::addLetters("", 10, base, modulo);
	return SpacedSeeds::clusters;
}

void SpacedSeeds::addLetters(string str, int tgt_depth, long base,
		long modulo) {
//	static int i = 0;
	if (str.length() == tgt_depth) {
//		cout << i++ << endl;
//		std::cout << str << std::endl;
		int hash = Hashing::calculateHash(str, base, modulo);
		SpacedSeeds::clusters[hash].setId(hash);
		SpacedSeeds::clusters[hash].addKmer(str);
//		SpacedSeeds::clusters[hash].kmerIds.push_back(i++);
		return;
	}

	for (int i = 0; i < strlen(letters); i++) {
		stringstream ss;
		ss << letters[i];
		addLetters(str + ss.str(), tgt_depth, base, modulo);
	}
}

} /* namespace ss */
