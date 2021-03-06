/*
 * LrcExactSeeds.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: prashant
 */

/**
 * 1. implement a rolling hash function					{hash finding function and rolling hash function}	[Done]
 * 2. use a definite base value and modulo (100, 197)	{will set the values and parameters} 				[Base = 100, P = 197]
 * 3. use a 12-mer to hash on							{have to hash based upon this pattern length}		[K = 12]
 * Things to do:
 * 4. start from the first base and find the hashes of each 12-mer for each read
 * 		using a sliding window							{function to slide over each read in a loop}
 * 5. find the modulo of hashes and put them in appropriate bucket of 197 modulo {built a class structure to store the frequency of the hashes over all the reads.}
 * 6. run the above steps for each read							{built a data structure of hashed should be good for hashing}
 * 7. After each read is hashed, we will obtain 197 clusters
 * 8.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include "Smithwaterman.h"
#include "ReadFastaFile.h"
#include "Read.h"
#include "Cluster.h"
#include "ValidateClusters.h"
#include "Cell.h"
#include "SpacedSeeds.h"
#include "Hashing.h"

using namespace std;
using namespace sw;
using namespace lcr;
using namespace ctb;
using namespace ss;

char bases[] = "ACGT";
int numReads = 0;
int numClusters = 0;

/**
 * Will write the clusters to a file with all the read ids
 */
void writeOutput(char* file, Cluster* clusters, int modulo) {
	ofstream myfile;
	myfile.open(file);
	int i = modulo;
	while (i > 0) {
		if (clusters->reads.size() > 0) {
			myfile << clusters->id << " : ";
			for (int i = 0; i < clusters->reads.size(); i++) {
				myfile << clusters->reads[i] << "," << clusters->kmerIds[i]
						<< " ";
			}
			myfile << " : ";
			for (int i = 0; i < clusters->kmers.size(); i++) {
				myfile << clusters->kmers[i].kmer << ","
						<< clusters->kmers[i].id << " ";
			}
			myfile << endl;
		}
		clusters++;
		i--;
	}
	myfile.close();
}

/**
 * To sequester the reads into different clusters depending upon the presence of a K-mer
 */
Cluster* sequesterReads(Read* reads, int kmer, long base, long modulo) {
	Cluster* clusters = new Cluster[modulo];
	for (int i = 0; i < modulo; i++) {
		clusters[i].id = i;
		numClusters++;
	}
	Read* temp = reads;
	while (temp->id != -1) {
		//cout<< temp->seq.substr(0, kmer).size();
		if (kmer > temp->seq.size()) { // If the read is smaller in length than seed length
			temp++;
			continue;
		}

		int hash = Hashing::calculateHash(temp->seq.substr(0, kmer), base,
				modulo);
		clusters[hash].setId(hash);
		clusters[hash].addRead(temp->id);
		clusters[hash].addKmer(temp->seq.substr(0, kmer));
//		cout << hash << endl;
		for (int i = 0; i < (temp->seq.size() - kmer); i++) {
			hash = Hashing::calculateRollingHash(temp->seq[i],
					temp->seq[i + kmer], hash, kmer, base, modulo);
//			cout << hash << endl;
			clusters[hash].setId(hash);
			clusters[hash].addRead(temp->id);
			clusters[hash].addKmer(temp->seq.substr(i + 1, kmer));
		}
		//	cout << "Read " << temp->id << endl;

		/**
		 * Code for calculating the hash values for reverse complement of the read
		 */
		string revComp = temp->getReverseComplement();
		hash = Hashing::calculateHash(revComp.substr(0, kmer), base, modulo);
		clusters[hash].setId(hash);
		clusters[hash].addRead(temp->id + numReads);
		clusters[hash].addKmer(revComp.substr(0, kmer));
		for (int i = 0; i < (revComp.size() - kmer); i++) {
			hash = Hashing::calculateRollingHash(revComp[i], revComp[i + kmer],
					hash, kmer, base, modulo);
			clusters[hash].setId(hash);
			clusters[hash].addRead(temp->id + numReads);
			clusters[hash].addKmer(revComp.substr(i + 1, kmer));
		}

		temp++;
	}
	cout << "Sequestering Done..!!!" << endl;
	return clusters;
}

/**
 * Will sequester the reads into clusters using a spaced seed
 * It is expected that each cluster will have reads corresponding to a single kmer
 */
Cluster* sequesterReadsSpaceSeed(Read* reads, int* spacedSeed, int len,
		long base, long modulo) {
//	SpacedSeeds seed(spacedSeed);
//	Cluster* clusters = seed.getClusters(base, modulo);
	Cluster* clusters = new Cluster[modulo];
	for (int i = 0; i < modulo; i++) {
		clusters[i].id = i;
		numClusters++;
	}
	Read* temp = reads;

	while (temp->id != -1) {
		if (len > temp->seq.size()) { // If the read is smaller in length than seed length
			temp++;
			continue;
		}

		int hash = 0;
		for (int i = 0; i < (temp->seq.size() - len); i++) { // for the forward strand reads
			hash = Hashing::calculateSpacedSeedHash(temp->seq.substr(i, len),
					base, modulo, spacedSeed);
			clusters[hash].setId(hash);
			clusters[hash].addRead(temp->id);
			clusters[hash].addKmer(temp->seq.substr(i, len));
		}

		string revComp = temp->getReverseComplement();
		for (int i = 0; i < (revComp.size() - len); i++) { // for reverse complement of the read
			hash = Hashing::calculateSpacedSeedHash(revComp.substr(i, len),
					base, modulo, spacedSeed);
			clusters[hash].setId(hash);
			clusters[hash].addRead(temp->id + numReads);
			clusters[hash].addKmer(revComp.substr(i, len));
		}
//		cout << i++ << endl;
		temp++;
	}
	cout << "Sequestering Done..!!!" << endl;
	return clusters;
}

/**
 * prepare a matrix with (clusters, reads, 0/1).
 * mark one for all the (cluster_i, read_i) if read_i is present in cluster_i
 * for any two reads, if they are present in the same two clusters, then they intersect
 */
Cluster* intersectClusters(Cluster* clusters, Read* reads, int crossParam) {
	Cluster* crossClusters = new Cluster[numClusters * numClusters];
	for (int i = 0; i < numClusters; i++) {
		crossClusters[i].id = i;
	}
	int numNewClusters = 0;

	for (int i = 0; i < numClusters; i++) {	// run the o(n^2) algo on each cluster and then merge them if they have more than the
											// given crossParam limit
		if (clusters[i].reads.size() > 1) {
			for (int j = i + 1; j < numClusters; j++) {
				if (clusters[j].reads.size() > 1) {
					for (int k = 0; k < clusters[i].reads.size(); k++) {
						if (clusters[j].checkRead(clusters[i].reads[k])) {
							crossClusters[numNewClusters].setId(numNewClusters);
							crossClusters[numNewClusters].addRead(
									clusters[i].reads[k]);
						}
					}
					if (crossClusters[numNewClusters].reads.size()
							< crossParam) {
						crossClusters[numNewClusters].flushReads();
					} else {
						numNewClusters++;
					}
				}
			}
		}
		clusters[i].~Cluster();
		cout << i << endl;
	}
	numClusters = numNewClusters;
	cout << "Number new Clusters: " << numNewClusters << endl;
	return crossClusters;
}

/**
 * prepare the overlap file using Smithwaterman algo inside clusters
 */
void prepareOverlapFile(Cluster* clusters, Read* reads, char* file) {
	ofstream myfile;
	myfile.open(file);
	OverlapMatrix overlap;
	for (int i = 0; i < numClusters; i++) {
		if (clusters[i].reads.size() > 1
				&& (clusters[i].reads.size() / clusters[i].kmers.size()) > 1) {
			for (int j = 0; j < clusters[i].reads.size(); j++) {
				for (int k = j + 1; k < clusters[i].reads.size(); k++) {
					if (clusters[i].kmerIds[j] == clusters[i].kmerIds[k]) {
						Smithwaterman* sw = new Smithwaterman(
								reads[clusters[i].reads[j] % numReads].seq,
								reads[clusters[i].reads[k] % numReads].seq);
						overlap = sw->prepareOverlapMatrix(
								(clusters[i].reads[j] % numReads),
								reads[clusters[i].reads[j] % numReads].length,
								(clusters[i].reads[k] % numReads),
								reads[clusters[i].reads[k] % numReads].length);
						myfile << overlap.toString() << endl;
						if (sw != NULL) {
							sw->~Smithwaterman();
						}
					}
				}
			}
		}
	}
}

void prepareMultiKmerOverlapFile(Cluster* clusters, Read* reads, char* file) {
	ofstream myfile;
	myfile.open(file);
	OverlapMatrix overlap;
	int multiKmerMatrix[numReads][numReads];
	memset(multiKmerMatrix, 0, numReads * numReads * sizeof(int));
	for (int i = 0; i < numClusters; i++) {
		if (clusters[i].reads.size() > 1
				&& (clusters[i].reads.size() / clusters[i].kmers.size()) > 1) {
			cout << i << endl;
			for (int j = 0; j < clusters[i].reads.size(); j++) {
				for (int k = j + 1; k < clusters[i].reads.size(); k++) {
					if (clusters[i].kmerIds[j] == clusters[i].kmerIds[k]) {
						multiKmerMatrix[clusters[i].reads[j] % numReads][clusters[i].reads[k]
								% numReads]++;
						if (multiKmerMatrix[clusters[i].reads[j] % numReads][clusters[i].reads[k]
								% numReads] >= 5) {
							clock_t startTime = clock();
							Smithwaterman* sw = new Smithwaterman(
									reads[clusters[i].reads[j] % numReads].seq,
									reads[clusters[i].reads[k] % numReads].seq);
							overlap =
									sw->prepareOverlapMatrix(
											(clusters[i].reads[j] % numReads),
											reads[clusters[i].reads[j]
													% numReads].length,
											(clusters[i].reads[k] % numReads),
											reads[clusters[i].reads[k]
													% numReads].length);
							myfile << overlap.toString() << endl;
							if (sw != NULL) {
								sw->~Smithwaterman();
							}

							cout << "Kmers: " << multiKmerMatrix[clusters[i].reads[j] % numReads][clusters[i].reads[k] % numReads];
							cout << " Read Ids: " << clusters[i].reads[j] % numReads << ":" << clusters[i].reads[k] % numReads;
							cout << " Read Length: " << reads[clusters[i].reads[j] % numReads].length << ":" << reads[clusters[i].reads[k] % numReads].length;
							cout << "Total Time in Seconds: "
																<< (double) (clock() - startTime) / CLOCKS_PER_SEC << endl;

						}
					}
				}
			}
		}
	}
}

/**
 * To cluster the reads read from the fasta file.
 * Basically acts as a wrapper around the sequesterReads method
 */
void clusterReads(char* fastaFile, int kmer, long base, long modulo,
		int* seed) {
	ReadFastaFile reader(fastaFile);
	Read* reads = reader.readFastaFile(&numReads);// reads the contents from the fasta file
	Cluster* clusters = sequesterReads(reads, kmer, base, modulo);// cluster the reads depending upon the prescence of Kmer

//	Cluster* crossClusters = intersectClusters(clusters, reads, 10);// intersect the clusters

//	Cluster* clusters = sequesterReadsSpaceSeed(reads, seed, kmer, base,
//			modulo);	// spaced seeds clustering


	 ValidateClusters validate(clusters, reads, numClusters, numReads, kmer);// Thats the test bed thing. Will validate the correctness of the
	 // hashing algorithm with TruePos, FalsePos and FalseNeg
	 validate.validate();
	 cout << "Kmer: " << kmer << endl;
	 cout << "True Positive: " << validate.truePos << "\nFalse Positive: "
	 << validate.falsePos << "\nFalse Negative: " << validate.falseNeg
	 << "\nTrue Negative: " << validate.trueNeg << endl;

	 char file[] = "outputLcr.txt";
	 writeOutput(file, clusters, modulo);


//	char overlapFile[] = "overlap.txt";
//	prepareOverlapFile(clusters, reads, overlapFile);

//	prepareMultiKmerOverlapFile(clusters, reads, overlapFile);

	// calculate average cluster size
	double total = 0;
	for (int i = 0; i < numClusters; i++) {
		total += clusters[i].reads.size();
	}
	double mod = numClusters;
	double average = total / mod;
	cout << "Average Cluster Size: " << average << endl;
}

/**
 *
 */
int main(int argc, char **argv) {
//	char file[] = "InputSequences.txt";
//	char file[] = "NC_000913.100k.pacbio_ec.reads.fasta";

//	char file[] = "reads.2k.10x.fasta";

	char file[] = "reads.2k.30x.fasta";
	// To calculate the time in seconds
	clock_t startTime = clock();

	int seed[] = { 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1 };
//	int lengthSeed=0;
//	lengthSeed=sizeof(seed)/sizeof(int);
//	cout<<"length of seed: "<<lengthSeed<<endl;

	clusterReads(file, 16, 4, 1299827, seed);// fasta file, kmer, base, modulo, seed

//	cout << calculateHash("GGCGGTAATGGCGTAATCGCACGGG", 4, 1299827) << endl;

	//	SpacedSeeds s;
//	s.addLetters("", 10, 4, 1299827);

//	string str1 = "GGCAGAAGACCTATAGCTGGGACACAAGGGGAGCTGGACGCTTGCCACCGAGTGGCCTGCCAGAAGCGAAAAAAAGAAGACGAGCTAGCCGTTCCACGTCGTGGCTTATGATTTTGGTGCCACAGCCGCACACATCCTGCGGATGCGGTGGATAAGGCAGGCTGTCGCGCTGACCATCGTTCCGGCCAAACTTTCATGCGGAACGATGTTTGCTTGAAAATGAATCCAAGACGGGCATCTTCCTCTCCAAGGTCCTGGCGACCCCGGCCCGTTCGCGAAAATTACGCCATCTATAACCGCCATCACAGAAATTGACCTCGAAACCGATATTCGGTATTCAGGCTCTGTCTCGGTCACAGCGCTGGCGTGGCGCAGCCGGTGCGAAAGACTGCTCAAAATGAACATTATGGTCACCTCGGCGGCAACACATTCTCGGTTAAAGATGTGGAGCAAAAACGTGGTAACTGACACGCCGCAGAACCCCACGGTTTTGCGGGTGAGACGAAGCAAACATATACCCTTGCAAACCTGCGTGTCACGCCATAATATCCCGCTTGGTTCGACCGTACGTGTACAGGGCATTCATCGCACCAGATTAAAACCGGCATCAGCCTCCAGGGGGCAGCCCATGAAGCCAGCCCTGCGTCCACATCGACCGTGCCGGCCGTTGTATGCGACCACTTTATCGAGTTATTGAAGTCAGTACCCGTAAAACGACTAAAGTAAAATCAGGTAGTAGAAGAGCCATGCCAAAACGTACAGAGTATCAAAAAGTATTGCCTCTGCAGTTCTGGTCTGCGGGCCCGATTGTTTATCGGTCAGGCGTGTGAGTTTGACACTCTGGCGCTGGCAAAGGTTGTAAAGCCCTCACGTGAAGAGGGTTTACGCGTCATTTCTGGTGAACGTCCAACCCCGGTCGACATCATGACCGACCCGGAAAGGCTGTATTGCAAACCTGACATCGAGCTCGATTCACTGGGAAGATTGATACCAAAGGATTATTGTAAAAAGAGCGCCGCGGTACGCGGGCTGCCAACGACTGGGCGGTCAGACGGCGCTGAACTGCGCCTGGAGACTGGAACGTCAAGGGCGTTGTTGGGAAGAGGTTCTGGTGTACACCATCTGACTGGTGCCACTGCCGAGTATGCGAATTGGATACAAGCAGAAGACCGCCGTTCGTTTTCGACGTAGGCGATGAAGAATAATTGGTCTGGAAACCGCGAGCGTTCCGGTATACGCCACCACACGATGGGAAGCAGCGCACTCGGCGGTTGCCACTGACGGTGGACGCCTTCCCCGGTGCATTATTCCGACCTCTCTTCCTAGTCCATTGGGGCGGTAGCTGCGACGGTTAATCGCTTATAACCGTGAAAGAGTTTGAAGCAAATTGAGCCCGCGGTCTGATGATCTCTCTTCTAACCAAATAGATTGCTGATTTGATGAGATACGCTAGATCGTGTCCTGGAAAGAGTACCGAGATGGTATAGTGGTGCGTGATAAAAACGACAACTGCCTCATGTCTGCTACTATTCGGAAAACTTCGATGCGTGATGGGCGTAATCCAGCACCCGCGTAACCTCCAATGCACTGTTCGGCCCCAGCCCAACGCTGACCGACATGAATATTCACAATCATGCGTAACGCCTCGATGGCGGTGCTGCGCATTGAAATCGGCGGTTGTAAACCGGTGGTTCCAACGGTCAGATTGTTGCGGTGGAACCCTGAAAAACGGTCGTTCTTGATTGTTATCGAATACTGAACCGAGCGCGTCGTCCCGTTCTTCGGCGCTGGTTCTCGAAGAGCGATCCCGGTTTCCCGGTTGACTAAAAGTACGGCGGCGAAACTGGCCGGTGGGTTGACACCCGCTCGACGAAACTCTGATGAACCACATCACCTGGCGGGCGTACTCCGGCCTCCTTCGAGCCGTGTGCCACTCGACTGATGTGGTTACTAAATTGCCTCGCTTCAACATTCGAAAAATCGCGGCGACTGCTAACGACGCGTCATGTACCAGCTCCAGATGAAATCCGGTTGGCGAAGTAGATGGCGATTGCTGGTCGCACGCACAGGATCCCTGCAAAAAGCGCTGCGCGGCCTGGGAAGTCGGTGCGACGTGGATTGACCGAAAAGTGAGCCTGGACCTGGACGCCGCAGAAGCGTTAAACACAAAATCCACTCGCGAACTGATATAGCAGGCGAAAGATCACGTATGCTGGTACCATCGCCGATGCGTTCCCTCGCGGGCCTGTCTGTGGACGGCCGTCTTCAACCTGATCCAAACATTTGACCTGCTGGTTCCTGGTAACAGAGTTTGAAGGAGACTGGTGCTGGTCAGGGGGAAAGCAGAATAGGGACGGAAGTGGGGCATGCACTGGCCTGCAACGCGAACTTCCTGCGCCTCCAGCTGGACAACGCCAAAGGGCTTTGACCGACTGCGCGCTTGGCACAAACACTGGCGGGCGTACGCCGAAGTTCGGAACATCTGTAGAGCTGCGTGACCAGTATGACGTGCACCTCGGTTTATAACAGCGTGGATGATCCTGGTGCGGCAGAGTTCGGCACCGACACAGGACTTACATGTACTCCAACATTATGAAGAGAAGAGATGCGAAGGTAATCCGTGCTAACGGACCGTGAAACAAAAAATAACATGGTGCTTGGCGGCGGCATCCGAACCCGTATCGGTCAGGGTTTGAATACGACTCACTCGTTGCGTACACGCCCTGCTGGCGCTGCGCTGAAGGAGCTGGCTTACGAAACACTATTATGGTGTAACTTGTAACCCGGAAACCTGTCTCCACCGACTAGACACTTCCAGACCGCTCTACTTCGAGCCGGTTAACTTGTGAAGATGCTGCGGAATCGTGCGTATCGACTGAACCCGAACATGCGTTATTCTCGTCCAGTACCGGCGTGTCAGACCACACGCGACAATGGCGCGGCCGCACTGGAAGCCCATGCTGGTGCGTAACAGAGTTAATCGGTCCTACAGCCCCGGATGCACTATTCGACCGTGCTAGAAGACCGTGAACGCTTTCCAAGCATGATCGGTTGAGTCGTCTGAAACTGTAAACAATCCGGGGCGAACGCCACCGTTACCCGACTACTTGAAATGGCGGGTAGAGAAGGCGTAAGGTAGGATCTGGCTACCCGCTGGCTGCGTACGTGCCGTCTTACGTTCTCGGCGGGTCGGGCCGAGCAAATCGTCTGATGACGAAGCTGGCCTGCGTCGTCTACTTTCTCCAGACGGCGGTCAGCGTGTCTAACGATGCGCCAGCTTGTTGCTGGACCACTTCCTGCGATAGAGCGCGGTAAGGAGTTAGACGTGCGATGCCATTCTGCGATCGGCGAAATGGCTGCCTGAATCGGGCGGCATCATGGAGCATATTGAGCAGGCGGTGCGGCACATCCGGTGACTCCGCATGTGTCTCTGCCAGCCTACACCTGTAAAGTCTAGGAAATTCAGGGAATGTGATGCGCCACCAGGTAGCAGAAACTGGGCCTTCGAATTGCAGGTCGCGGCGGCCTGAGTTGAACGTGCGAGGCTTGAACGGCTGAGAAAAACGAACGAAGTTCTACCTGATTGAAGTTCAACCCGCGTGCGGCGCAGTACCGTTCCTGGTTCGTCTCCCCAAAGCCACCGCGCGTACCACGCTGGCAAAAAGTGGCGGCGCCGCGTGATGCGGCAAAGTCGCGTGGCTGGAGCTGGGCGTGTAGCTCAACGCGAAGTTACTCCCGGCCGTTTACTAGTCGTCGGTGAAAGAACGTGGTGCTGCCGTTCAATAAATTGCCCCGGGCGTTGACCCGCGTTAGGGTCCAGAAGATGCGCTCATACCTGGGGAAGTGCATGGGTTCGTGGGGCCGGCACCTTTCGTGAAGCGTTGCCAATAGACGCAGCTGGGCAGCAATCCACCATGAAGAAAGTCCGTGTCGTGGCTGCTTTCCGTTGCGCGAAGGCGATAAAAGTAACTGCGGGTGGAACCTGGCGGCAAAACTGCTGAAACCAGGGCTTCGAGCTGGATGCCGACCCACGGCACGTCAGATTGTGCTGGGCGAAGGCAGTATCAACCCCGCGTCTGCGTAAACAGAGTGCATGAAGGCCGTCCCGCACATTCAGGACCTATTCAATGCAAGTGGCGAATTATACCCTACATACAGTCACACACCACCTCCAGGCACGTCGTCGCGATTGAAGCACTCCCGCAAGTAGATTCGTCGCAGTCGCGCTTGCGAACTTATAAAGCTGCATTACGACACCACCCTGAACGGCAGGCTTTGCCACCGCGATTGGGCGGCTGAATGCCGATGCCGACTAACAAAAAGTAATTTACGGTGCAGGAAGTGGCACGCACGATCAAATCAATAGCGTGTCAGTTTGGCAGATTATTTTTCATCCGCGTAATTATGATGAATAACTAATACGGGTTCACTGATAGAGGACCGTTTTTTTTGGCCCATTAAAGTAAAATTCCTGTTAGGGGAATCTGAGTACTTTTTGTATGACATAAGCACGGATATATAGCTCGGACACTATATCGACGTGAGTTGCATAAAGTATCGATTATATACGTCACCTAATTTCTTAATATGAGCCTTTGTGCTAATTAGCTTCAAAACTGCAGTCATAGCCAGACTTTTTAATTTGTGGATGAAACTGGGTTCGTAGGTGTGATAGAATATGTTGAAAAAACCAACTCTACTTGTTAATCGCCGAGATGGATGATTGACTAGAAAATCGGTGGGGTGATAAGCAAGAGAGATCTCACTATTCATTTCGATATTGACACAGCAGAGGCGGTTCATAACCCTCGACTTAATCATTCTAGTCGGAAGTCACAGATAATAAGCTGCGAAGTTTAATGATGATTCCCTTAAAATAAGCTGGAAGGGACCTGGGATGCCAGCGTCAGCGACGGTATAAAGTGGCGATTATCGATTGAGTGCAAAATTTACGCCGCGCGCTGCGCAAATAAAATGTCGGAGAAAAATTAGTCGGTGTCAAGAAAAGACGTCGCGTCATTCAGCTGCCGGTCTCCGTCACGGATATTAACCGCGGTAGCTAGAAGTCGGCAGATGCATGTTGCGAAAGAGTATGCGACTCATTAATTTTATCTCGTTGATCCGGGCGTCCTGCTTGCCAGATGCGATGTTTAGCATCTTATGCCAGTCCTAACCAGGTCGCATCCGGAAGGATCCACTCGTTTATGGCGGTCACATGCCGTCGCCCCCTACGCAAACGGGGCGAATAGTCTTTGCCCTCCATTTCCTCAGTGGCTGCGTTCATGTAACGATAACATAGGGACAGCGGCCGACAAGATCCTGATACTGCCTTTGGTATACGAACTCGTTTCCAGTTGTAACTCATCGTCACTAACTATTGCGTACAGCGCGGGCCTGCGCGTACCCAGTCAAGCAACATGGTCGTTTCTCGCCGCGAAAGCCCGCTTTGACAAAGCTCATGGCGGCAAACAATGCTCTCTCTATCGCCAAATGACCGCGTCCATCCATATCACGCTGTATCCATCCGTACCAATGACATCGCGACCAATCAAAACCAAGCCCGTGCAGGATACGCTCGCTGCCCGATCATTGGCCGTTTTCCCGAAACGATACGCTGTCAGGTCGCAGTAGCATGCATAATGCAGCCTATCATGTAAATTGGCTCCCGCTTTATGGCACGATTTTCAACGCCCGTAGGTCCACCACGCAGTGAGGCGACGTGGGCCGCATGATGCTACCGGCTCGCACACATAATACCATCGCCAATTCAAGACGGGCACACTGGTGGACTGGACAGAAACGCTCGTCGGGTGACACCAACCGGAATTAACTCCCTTCCAAAGGCGTAATACGCTTCACGGTTGATTAAACATGTCCTTTCCACAACCGGAGCGCGCTTCTCGCGCCAAACGCCAGCGTGAGCCCTTTCAATGGCCATCGTTCCTGAATGCACGAAACGCGATGTGATAGTGTTTCAACACTGGCGCCTGCCGACTTAGCACGATACGCTTCTTCTTACCGGCATTTCGCTGAGTTGGTGCGGTAGATCTCTTTCCAGCGTCGCAGATCGACAGCGGGGCGCTGTTAACCAGCTGCTGAGCCAGTTCGACGGGCTATATCCGATCAGTTCCGCCTGGCTAACCCTACGCGGGTTAGACTATCCCCGCACGGCAGCGTCCTTTCTGCGCCCATTACGTCTGCCGCGTCATCAGCCATTATCATTGACGATAGCAGGCGAGCAACAGGATCTTCGTTGTTAGATCGGCAGCACACCGCGGCTGTCAGGAACGATCGCCCAGTTTGTGCTTGCGCCGGCAGGGCGAAGCTGGACGTTATCAGGCACAAACAATAAAATCTGCCGCAGCGCCATGTTCAAAGCCTGCCGCCAAAGGCATAGCGTTCTACAGCTGCGCACTAATCCGGTTTGTCCGCACATTGGAAATATGCTCGGTTAATATCCCGCAAAAACCACCTCGGACCGGAAAGTCAGGCATCCGGCTGCTTCGCCTTCTTAGTGCCGCTTTTCAACATCCCACGCCCCGCGGAAAAGAACTACTCTCTCCGGCACCGGTAATAATGGCGACACGTAAATTGCGGATCGTCAGGAAATTTGAAACAACTTCGCCCATTTCAAAGCCTGGGTTTGTGCATTCAATAGCATTCGCTTTTGGACGATCAA";
//	string str2 = "ATCGCACCGATTGCGAAGAAGCCGCTGCGCGGTTGCGCGCTTGACGTGGGCTTTCCCGATGCATTATTTCGCCGATGCCTTTACCATGGGCAGGTAGCGTGCGGCGGTTATCAGCTTACTAACCGTAAGAGAGTTTGAAAGGAATTTATGCGGCCCCCGCGGCAATCTGGATCTCTCTCCGACCAAAGATTGCTGAGTGTGATGAGTCGCTGACCGGCTGGTAAAGAGTACGAGAAATGGAAGTGGGTGCGTTGATAAAAACTGCCAACCTGCATCATCGTCTGCCTCTATCAGTAAAACTCGATTGCGATGGGCATCCACATCTCGGTGACTCCATGCACTGTCGCGCCAAGCCCAAACGCTTGACCGGACAAAAGAATATCAAATCGATGTCGTTGAACGCCTCGACATGGCGGTGCCGTCGTGAAACTCGGCGTTGAAACCTGTGTGGTTCCAACGTTCAGTTTGCGGTGTAACCCGTGAACAACGGCGTCTGATTCGTTATCCGAATGAACCCATCGCGTGTACCCGTCTCTTCCGGCCAAGCTGGGCGTGCCGGAAAGCGAACACGCCGTTTCCCGATTGCTAAAAGTGGGACGGCAGAACTGTGCGGTGGTTACACCCTCGAACGAACTGATGAACATGACATCGAGCCTGCGCGAGTACCTGCTCGGGCCTCCTTTCGACCGTCCATCGACTGATTGGATGGTTACTAAAAATTCCTCGCTTCAACTTGCTCGAATAAATTTGGCGCCGGTCGCTAAGCGACCGGTCTGACCATCTCAGACTGAAAATCGGTTGCGAAGTGATGGCGATTGGTCCGCACGCGAGCAGGAATCCCTGCAAAAGAGCGCTGCGCGGCCTGAAGTCGGTGCGACTGGATTGCGACCCGACAAGTGAGCCTGGTATGACCCGGAAGGCTGTCTAACCAAAATCCGTCGCGAACTGAAAGGCACGCCAGGCGCAGATGCGTATCTGGCTACATCGGCCGTGCGTTCCGCTGCGAGTCCTTGTCTGTGAGACGGGCGACTTCAACTGACCAACATTGACCGCTGTTTCCTGGCTACGATTTGAAGAGCTGGTGCTGCTCATGTAAGAGAAATGGCCGGAGAGTGGGCATCACTGGCCTGTAACGCTAAGACTTCCTGCGCCAGCTGAAACGCAAAGCGCTTTGCCTATGCGGCGGCTTGGCAAACTGACGGGCGTACGCGAAGCGGAAATCCGTAAGCTTGCTGTGAACCAGTATTAGACGCTAGGCACCCGGTTTATAAGCCGCGTGATACCTGTGTGGCAGAGTTCGCTCACCGACACCGCTTTACATGTACTCCACTATGAAAGCAGAGTGCGAATCGAACTCCGTTCTACCTGACCGTGAAAAAATCATGGTGCTTGGCGGCGCCCGTACCCGTATCGGCTCAGGGTATCGAGATTCGACCTACTGTTGCTACTACATGCCTCGCTGGGCGCATGCGCGGCAAGTACGGTTTACGAAACCAATATCGTTAACAGTAACCGGAAAGCCGTCTCCACCGGACTACGACACTTCCGACCGCACTTACTTCGAGCCGGTAACCTCTGGAAGATGTGGCTGGAAATCGTTGTCGCTATCGAGAACCGAAAGGCCGTTATGTACCAGTACAGCGGTCAGACCGCACCGTCGTGAAACTGGCGCGCGCGTCTGGAAGCTGCTGGCGTGACCGGTCACTCGCAACCAGCCCGGATGCTTATCGACCGTGCAGAAGACCGTGTACGCTTCCACATGCGGTGAGGCGTCTGAAACGGAAACAACCGGCAGAACGGCCCAGCCGTTACCGCTTATTGAAATGGCATGTGTAAGCCAAGGCGAACAGCGATTGGCTACCCGCTGGTGAGTACGTCCTCTTACGCTTCTCGCGGTCGGGCGATGGAAATCGTCTATGAGCGAAGCTCGACCTGCGGCGCTACTTCCAGGACGGGCGGTCAGCTGTCTAACGATGCGCCAGTGTTTGCTGGAACCACTTCCTCGATGACGCGGTAGAAGTTGACGTGGATGCCCATCTCGCGACGGCGAAATTGGGTGCTGGATTGCGCGGCATCATGGAGCATATTGGGAGCAGGCGGGCGTGACTTCCGGTACCCGCATGTTCAACTGACCAGCCTACACCTAAAGTCAAGGAAATTCTAGGGATTTGCATGCGGCCAGCAAGGTGCTGAGAAACTGGCCTTCGAAGTTCCAGGTGACGGCCGCCTGATGAACGTGCAGTTTGCGGGTGAAAAAACAACGCAATGCTCTACCTGATTGAAGTTAACCCGCGTGAGGCCGCGTACCGGCTTCCGTTCGTCTCCAGAGCCACCGGCGTAGCCGCTGGCAAAACCGTGGCGGCTCGCGCTGGTTGATGGCTGGCAAATCGCTGGCTGCAGCGAGGGGCGTACCCAAATGAAAGTATATCCCAGCCGTATCTCACTTCGGTGAAATAAGTGGTGCTGCCGTTGCAATAAATTCCCGGAGCGTTTGGACCCGCATGTTAGGGGGCCAGAAATGCGCTCTAGCCGGGGAAGTCATCGGGCGTGGTAGGTCCGCACCTTCGCTGAAGGCCGTTTGCCAAAGCGCTAGTGGGCAGCGAACTCAAACACCATGAAAAACACGGTCGTTGCGCTTGCTTTCCGTTGACGCTCGAAGGGATAAAGAACGCGTGGTGGACCTGGCCTGCAAATACTGCATGAAACAGCGTGGCTCGAGCTGTGATATGCGACCCATGCGGCACGGCGATTGTGCTGGGCGAAGCCAGGTATCAACCCGCGTCTCGGTAACACAAGGGTGACATGAAGGCCGTCCGCTAGCATTATCAGGAGACCGTATACAAGAATAGGGCGAATAATACCTACTATCTATGCAACACCACTTCAGGCCGTCGTGCGCATTGAAGACCTCCCGCGTGAGATCGTCCGCAGTGCTGCTGCAATATAAAGTGCATTACGCACACCACCCTCGCAAACTGGCGGCTTTGCCACCGCGATGGGCGCGCTGAATGCCGATGCGACTGAAAAAAGTAGATTTCGGTGCAGGAATATGCACTGCACAGATCAAATAATAGCGTGTCAATGGCAGATATTTTCTCATTCCCAGACTAATTTGGATCGCGAATAACTAATACGGTTCATCTGCAGTGAGGACCGTTATTCTTTTTAGCTACACATTAAGTAAATCCTTTTGGGGCGTAATCGATATTTTTTGATGACATGAAGCAAGGATTAGCTCACACTTAACTGACGGTGAAGTTCCATACTATCGATATATCCACAATTCTTAAGATGGCCTTGCTTTAAATTGCCTTTCAAAACCGGAGTCATACCTGCTCAGACGTTTAATTTGTGAATACTGGGAGTTCGCTATTGTGAAGGATATGTTAAAAAAACCACTCTCTCTGTTAACTCGCCGAAGTGGTAGATCGCTGAAAATCGCGTGGGTGATAGCGAAGGAGAGATCTGGCTATTCATTTCCGGATATTGACATCACAGCACGGCGGTTAATACCAACTGATCTTATGACTTCTGTCGGAAGTCAGCAAGAAATAATGGCTGCGTAAGTAAAGATGATCCCCGTAATAAGTTGGCGAGGGGGGGATGCCAGTGTCAGCTTAGACTGGCCTTAAGAGTGCGTCAGATAATCGGATGAGCAAATTTAACGCGCGCATGCGCCAATACAGTCGGGAAAAAGTTATCGGTGTGAAGGAAAGCACGTCCGCTTATTCCTGCCGTTAGATCCACCGGAACTTACACCGCGAGCACGAAGTGGCAGATGGATGTTGTCCAAAGAGCATGATGCGTCGTTAATTTTTATCATCGTTGAATACCGGGCCGTCCTGCTTGACCAGATGAGATGTTGTAGCATCTTATTCCAGCAACTCAGGCTCGCATCAACGGCAAAATCATCACGGTTTAGGCGTCGCATCCGTCGTCCCCCTGCAAACGGGGGCGATTTTCCTCCATTTGCCCCAGGTGGCTGCTGTTTCATGTAACGATACATGACAGTCGCCCGACAAGGATCCATGATACTGCCTGTATTGGTGCTTAACAGTTTTCCAGTGTATACTCGCTCGTCACTAAGCATTGCGTACAGCGCTGGGCTGCGTCGTACCCATCAACCAACTGGCGCTTTCTCGCCGCGAAAGCCCGCTTTGACAAAGCTCATTGGCGGCAACAATGCTCCTTGGCTCAAATGAGACCGCGCCATCCATACATCAACGCTGCTCATCCCCGACCCAATGCTATTCGCGACCAATCAAACACAACCATGGCAGGATCTCTGCTCAGCGTGCCCGATATGGCCCGTTTTCCCGCAACGATAGTGTCAGTGTCAGCAGCTAGGCCTATGCCATAAATAGCACGCCGATACCTGAATATCTGGCTCCCGCTTGCACTGTATCAACCGCCCGTAGTCACCACGCAGTAAGGGCAGTGTGTGACCAGTAGACACCGCGCCTCCCACAACTTACATCGCCGAATCGGAACGGACGGCACTGGGATGCCACAAACGCCGTCGGGATGAACCTACCCGGAAGTAACCACCTCCAAAGGGGTAATAGCTCTCCGGTTGTTAACGTCCTTGTCCACACGCGGATCGCGCTTCCTCGGCAAACGCCAGCGGCCCCTTCAATGAGCATCTTCCGAATGCGAACCAGTTGGATACCGGTGTTTCCAACATCGCCCCATGCAGCAATTATAGCGATACGCTTCTTCTACCAGGCATTTCGCATGTGGTGCGGTAGAACCTCTTTCAGCGCGCGCAATCGGCCAGCGGGGGCGTCTGTTGAACCAGCTGCTGAGCCAGTTCGCGGGCAGTTTATCCATCAGTTCCCCCTGTCTAAACCCCCACGCCGGTTAGGACATATCCCCCAACGCCAGCTGCCTCTTCTGCGCCCATTCGTCTGCCCGTCATCACCGAGTTTCATTGACTGATGGACAGGCGGCAGTTGATCTTGGCCAGACGCAAGCACACCGCCGCTGTCAAGGAACGATGCCCAGTTTGGCTTCCAGCAAGGGTCGAAGCTGCGCTCGTTATCAGGCAACAAAACCATAAAATTCTGACCGCCAGCGGCACAGTTCAAAAGCCGGCCGCCTAAAGGCATAGCCGATTCACAGCTGCGCAGTAACCGGTTTGTGATGATTGAAAATTCGGTTATATCCCGCAAAACCACCCCCCGGACCATAAGTCAGCATCCGGCGTGCTTCCGCCTTCTGCTGCCGCATTTATACAGACCCAGCGCCCGCGGAAAAGAACTGTCTTCTCCGGCACCGGTAATAATGGCGACACGTGGAATTGCGGATCGTCACGGAAACTTTTAGAAGAATACTTCCCGCCCGATATTCAATAGCTGTGATTTTTGCATCAATATCATTACGCTTTTGGCACCGAACTCAAGGGTAATTTCCAGAATTGATCCATTAGCGGGACATGATGTAATGATTCGTTAGCTCATTCCTTTTCATCCATTTTCTTGCTTTTTCAGGTGAGACGCTACAACATCCCTGGCAATAAAATGCATATTGTTTCTATTTAGACGTGTAGATTATTAGCTGGCAGGGTAGTTCCCTGCTGTTTCATTTATTTTCAGGTTCCTTGTCTAATTATATTCCCCCGAGCCAAATTCGTGGCAGATCTTTTCGTGACTCCAGATAAGAGGGCAAACTTTAAAAATCTTCGCCATCATTTTGTTCGCAGAAGGCGAGATAAAATTCCCTCTTCGCTCAATGTTTCGCCTTCATTCAGCAGCACAAATGCTTTTGTGGCTTCAACGCGAATCGAATCTTAAACCCACCAACCACTGATGTCACTGAATTTATCGGGTGCGCGGTCGATAATATTTGTCAGCTCCACCCGCTAGGAGACATTCTCGCCGCCACGTTATTTAATCATATTTGCAGCGGCGGAATCCGAACGAACATAAAAAAAGTCGTCTTCGTCGGCAGGTAATCCGGATCGCTCGGTAATCAGCCTAGCCCATCGGCTTCCCAGCACTTTCCGCAGTGGCTTGTGGGTGTTGAGAAAGTACTCCCTTTGTAAGATGGTATTTCCCAAGGATAATGCCTTTTATATGCCTGATTTCACCGATCTACACCACGCGGGGAGCGGGCGATTGTGATCGTCGCGGAGTCTCCGCTTCGGTAGCGAAAAACCCCACCCGAGCCAATCGGACGGCAGGTCGTTTATCGCACAGGGACGATACGCCGATAATGCCCACCAATGGTTATCCGTCATCCCATAAGACGTCAGCAAAGCAGATACGCCGAAGCGGGCGACTAAATACGGCGTCTTTTACGCTGCTCCGACAACGTTGAGAAAAGACATCACGTTCCCCGCAGGCGGTGTTGCAATGAGTCGGTTCGCATGAAGGCGGCTGTACCATCAACAACGTACGGATCATCCATCGGATACAATTCGGGTACGGTGGCGGACGGTACTTCTGTAACCTTGTCCGCCAGAAGAGCAGCGGGCGCTGTTATTCTCGACTCAGCCAAATCGGGCCCCGGCAGAAAACGCCGCACATCCGCCGCAGTACACTGGCGAATCTTGATTATGGAAACGACAGGCATGACGTGTCAGGTAGACCGTCATCGTCACTACTTAGTGCGTACCACTGCCAAGGGCGGAGTAACTAATCCAAGCGGGAAAGGCAAGGTTGTAATGGCGTATCCAACCCACATTACCTTTCGGGTCTCTGGAGGTGGGTGCACCGGGAAGGGTGAAAGAGAATGTTCCGCCGTATCGTCAGTTCGGAATAGCGGCGGTGCATAGCACTAAAGGTGGCATGTTGTTTGGACTATCTTCAGATTGAAGTAACACGAACTCACGCCATCATCAGCGGGAAGTGCCACACATCTGCTCAGGCAAATGTGCCGGCAATTGAAGTGGCATCTTCTGCTGAATCTGTTGATACATAGTGATAGAATTGCGCACTGGTCGCCAGCAGGCAGCCCTGACTTATGTTGGCAGGATCCACGCGCTTTTCCTCGCACCAAAGGCGGGCGTTTAAATCGGCTACCATTAATCGCGCCAATTTTTGCCAGCGCCGATAACCAGCAAAAGATAAAGTTCTCGGGCAGTTGCTCAGATAGTAGTGCAACCTTGTACGCCTTTGTCCGAATCCCCCAGCGTATAATAACAGGTTTGCACGTGCGTGTTAATCTCCTGATTTAACTCAAGATAACCATACCGGTTAACGGACTCCCGCCGCTGGATTCACCAAATCTGCGCCCGGTTTTATGATCCGTAAACGTCCACAAGATCGTCCCACATTTGACGTAGATGTTGTCCGCCTAATGATATTCAATTGCACCTCTACTCCATTTTTTGTTCGTTTGTTATTGGGCGGGGCCGCTAGTCAGGCAAGCCGACTGACTGACCACGCGGTTGTAGGTCCTCAACTTTGGCCAGACCTATCATGCTGTCACCAACTCCTGAATGTCGTTTTTCGCCTGTAGCCTCGTATATTTACTTAAAATCGGCAGCACTGTACCCATGCCATGTGGAGGGCATTCCGCAGCCAAGATTTGTCCGGGGTTATTTTATGAAATTGTCGGCATGATGATTCCGGCCCTTTGCAGTGCGCTAACCAGTCCATCGTTTCGCCACTGAGTGGGATTCGATTATCGCCGGCCACATTATCTGTGGATTGCTTTCCAGTTCCGGTACTGGTCAGCATCTTTGGCGCAGGCGAGGTATTCTATGTTCAGCAAAGCGTTCTTTTACTTACGGCGCCGATGGGTATGTGGTCGCCAGCCAGTGCATGTAGTTTCTCTTCATAACGTCAGTGGGCCGTAAGGGACATTCGATACGGAATGGATAAGCTGCAGTTGCTCTTCCTGGGACTTTCCTGGGCTGTGCCAAGGCAGTGTAGTCGAAGGCTCAATATCTTTATAAGCACTCTTAATTTGGGTGAAATGCCCACCGAGTTCCATCCTCCGATGTAGCCGTCCGGCACAATTTAATAACAGACCGAACCGGCGTAGGTAGGGATCCGTTACCTTTGCTACATGCGCGGCACATTTTGGCCGCCGTTGAAGTAATGCATCTATGAATGTACTCGGCCCATATCGGCAGCATCACTTCAGTACATGAGCGAAAGTCGATACTTTCGCCTTTAGCCGGTTTCAACGCCTTTAATGCCCAGGTGCTTGCCGAGAGCGCGCCGTGCGTGGCGGTCAGGCCAGAAAAGTCAACGGCGGTATGCGGGAAGGCAGGCATTTGGATGGTCAA";
//	string str2 = "AGTTATGTCAGGGGACACGA";
//	Smithwaterman sw(str1, str2);
//	OverlapMatrix mat = sw.prepareOverlapMatrix(1, str1.length(), 2,
//			str2.length());
//	cout << mat.toString() << endl;

	cout << "Total Time in Seconds: "
			<< (double) (clock() - startTime) / CLOCKS_PER_SEC << endl;

}
