/*
 * ValidateClusters.cpp
 *
 *  Created on: Nov 17, 2013
 *      Author: prashant
 */

#include "ValidateClusters.h"

namespace ctb {

ValidateClusters::ValidateClusters() {
	ValidateClusters::clusters = NULL;
	ValidateClusters::reads = NULL;
	ValidateClusters::numClusters = 0;
	ValidateClusters::numReads = 0;
	ValidateClusters::kmer = 0;
	ValidateClusters::falseNeg = 0;
	ValidateClusters::falsePos = 0;
	ValidateClusters::truePos = 0;
	ValidateClusters::trueNeg = 0;
	ValidateClusters::multiKmerMatrix = new int*[1024];
}

ValidateClusters::ValidateClusters(Cluster* c, Read* r, long nc, long nr,
		int kmer) {
	ValidateClusters::clusters = c;
	ValidateClusters::reads = r;
	ValidateClusters::numClusters = nc;
	ValidateClusters::numReads = nr;
	ValidateClusters::kmer = kmer;
	ValidateClusters::falseNeg = 0;
	ValidateClusters::falsePos = 0;
	ValidateClusters::truePos = 0;
	ValidateClusters::trueNeg = 0;
	ValidateClusters::multiKmerMatrix = new int*[1024];
}

ValidateClusters::~ValidateClusters() {
	// TODO Auto-generated destructor stub
}

int** ValidateClusters::getMultiKmerMatrix() {
	return ValidateClusters::multiKmerMatrix;
}

/**
 *
 */
void ValidateClusters::validate() {
	int numOverlap = 0;
	int readMatrix[numReads][numReads];
	memset(readMatrix, 0, numReads * numReads * sizeof(int));
	for (int i = 0; i <= numReads; i++) {
		for (int j = i + 1; j <= numReads; j++) {
			if (reads[i].start < reads[j].start
					&& reads[i].end > reads[j].start) {
				if ((reads[i].end - reads[j].start) >= kmer) {
					readMatrix[i][j] = 1;
					numOverlap++;
//					cout << i << ":" << j << endl;
				}
			} else if (reads[i].start < reads[j].end
					&& reads[i].start > reads[j].start) {
				if ((reads[j].end - reads[i].start) >= kmer) {
					readMatrix[i][j] = 1;
					numOverlap++;
//					cout << i << ":" << j << endl;
				}

			}
		}
	}
	cout << "Actual overlap count (based upon fasta file): " << numOverlap
			<< endl;

	int clusterMatrix[numReads][numReads];
	memset(clusterMatrix, 0, numReads * numReads * sizeof(int));
	for (int i = 0; i < numClusters; i++) {
		if (clusters[i].reads.size() > 1
				&& (clusters[i].reads.size() / clusters[i].kmers.size()) > 1) {
			// cout << i << endl;
			for (int j = 0; j < clusters[i].reads.size(); j++) {
				for (int k = j + 1; k < clusters[i].reads.size(); k++) {
					if (clusters[i].kmerIds[j] == clusters[i].kmerIds[k]) {
						clusterMatrix[clusters[i].reads[j] % numReads][clusters[i].reads[k]
								% numReads]++;
					}
					//					clusterMatrix[clusters[i].reads[j]][clusters[i].reads[k]] = 1;
				}
			}
		}
	}

	for (int i = 1; i <= numReads; i++) {
		for (int j = i + 1; j <= numReads; j++) {
			if (readMatrix[i][j] == 1 && clusterMatrix[i][j] >= 2) {
				truePos++;
			} else if (readMatrix[i][j] == 0 && clusterMatrix[i][j] >= 2) {
				falsePos++;
			} else if (readMatrix[i][j] == 1 && clusterMatrix[i][j] < 2) {
				falseNeg++;
			} else if (readMatrix[i][j] == 0 && clusterMatrix[i][j] < 2) {
				trueNeg++;
			}
		}
	}
}

} /* namespace ctb */
