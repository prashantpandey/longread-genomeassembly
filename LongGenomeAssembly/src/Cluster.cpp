/*
 * Cluster.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: prashant
 */

#include <vector>
#include <iostream>
#include "Cluster.h"
#include "kmer.h"

using namespace std;

namespace lcr {

Cluster::Cluster() {
	Cluster::id = 0;
	Cluster::reads = vector<int>();
	Cluster::kmerIds = vector<int>();
	Cluster::kmers = vector<Kmer>();
}

Cluster::~Cluster() {
	// TODO Auto-generated destructor stub
}

Cluster::Cluster(int id) {
	Cluster::id = id;
	Cluster::reads = vector<int>();
	Cluster::kmerIds = vector<int>();
	Cluster::kmers = vector<Kmer>();
}

int Cluster::kmerId = 0;

void Cluster::setId(int id) {
	Cluster::id = id;
}

int Cluster::getId() {
	return Cluster::id;
}

vector<int> Cluster::getReads() {
	return Cluster::reads;
}

void Cluster::addRead(int read) {
	if (!checkRead(read)) {
		Cluster::reads.push_back(read);
	}
}

vector<Kmer> Cluster::getKmers() {
	return Cluster::kmers;
}

void Cluster::addKmer(string kmer) {
	int id = checkKmer(kmer);
	if (id == -1) {
		Kmer temp(kmerId, kmer);
		Cluster::kmers.push_back(temp);
		Cluster::kmerIds.push_back(kmerId);
		kmerId++;
		return;
	}
	Cluster::kmerIds.push_back(id);
}

int Cluster::checkKmer(string k) {
	if (Cluster::kmers.size() > 0) {
		for (int i = 0; i < Cluster::kmers.size(); i++) {
			if (Cluster::kmers[i].kmer.compare(k) == 0) {
				return kmers[i].id;
			}
		}
	}
	return -1;
}

void Cluster::removeRead(int read) {
	for (int i = 0; i < Cluster::reads.size(); i++) {
		if (Cluster::reads[i] == read) {
			Cluster::reads.erase(Cluster::reads.begin() + i);
		}
	}
}

bool Cluster::checkRead(int read) {
	if (Cluster::reads.size() > 0) {
		for (int i = 0; i < Cluster::reads.size(); i++) {
			if (Cluster::reads[i] == read) {
				return true;
			}
		}
	}
	return false;
}

void Cluster::flushReads() {
	Cluster::reads = vector<int>();
}

void Cluster::toString() {
	cout << Cluster::id << ":";
	for (int i = 0; i < Cluster::reads.size(); i++) {
		cout << reads[i] << " ";
	}
	cout << endl;
}

} /* namespace lcr */
