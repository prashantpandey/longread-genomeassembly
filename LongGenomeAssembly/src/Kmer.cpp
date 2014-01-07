/*
 * Kmer.cpp
 *
 *  Created on: Dec 1, 2013
 *      Author: prashant
 */

#include <iostream>

#include "Kmer.h"

using namespace std;

namespace lcr {

Kmer::Kmer() {
	Kmer::id = 0;
	Kmer::kmer = "";
}

Kmer::Kmer(int id, string k) {
	Kmer::id = id;
	Kmer::kmer = k;
}

Kmer::~Kmer() {
	// TODO Auto-generated destructor stub
}

int Kmer::getId() {
	return Kmer::id;
}

string Kmer::getKmer() {
	return Kmer::kmer;
}

void Kmer::setId(int id) {
	Kmer::id = id;
}

void Kmer::setKmer(string kmer) {
	Kmer::kmer = kmer;
}

int Kmer::compare(Kmer kmer) {
	return Kmer::kmer.compare(kmer.kmer);
}

} /* namespace lcr */
