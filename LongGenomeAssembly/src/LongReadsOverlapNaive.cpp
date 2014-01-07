//============================================================================
// Name        : LongReadsClustering.cpp
// Author      : Prashant Pandey
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include "ReadFastaFile.h"
#include "Read.h"
#include "ReadOverlap.h"
#include "Smithwaterman.h"

using namespace std;
using namespace ro;
using namespace sw;

/**
 * Will read the given fasta file for the reads
 */
Read* getReads(char* file) {
	int numReads = 0;
	ReadFastaFile reader(file);
	Read* reads = reader.readFastaFile(&numReads);

	return reads;
}

/**
 * Will return the complementary base pair
 */
char revComplement(char a) {
	char reverse = '\0';
	switch (a) {
	case 'A':
		reverse = 'T';
		break;
	case 'T':
		reverse = 'A';
		break;
	case 'C':
		reverse = 'G';
		break;
	case 'G':
		reverse = 'C';
		break;
	default:
		break;
	}
	return reverse;
}

/**
 * Will return the reverse complement of the given read
 */
Read findReverseComplement(Read a) {
	Read* revRead = new Read();
	string revSeq;
	for (int i = a.seq.length() - 1; i >= 0; i--) {
		revSeq.append(1, revComplement(a.seq[i]));
	}
	revRead->setId(a.id);
	revRead->setSeq(revSeq);
	return *revRead;
}

/**
 * To find the overlap between any two reads
 * The overlap will be of the type Suffix of read 'a' and prefix of Prefix of read 'b'
 */
ReadOverlap findOverlap(Read a, Read b) {
	long overlap = 0;
	ReadOverlap* readOverlap = new ReadOverlap();
//	cout << a.seq.size() << " " << b.seq.size() << endl;
	for (int i = 0; i < a.seq.size(); i++) {
//		cout << a.seq.substr(i, 1) << b.seq.substr(0, 1) << endl;
		if (a.seq.substr(i, 1) == b.seq.substr(0, 1)) {
			overlap = 0;
			int j = i + 1, k = 1;
			for (j = i + 1, k = 1; j < a.seq.size() && k < b.seq.size();
					j++, k++) {
//				cout << a.seq.substr(j, 1) << b.seq.substr(k, 1) << endl;
				if (a.seq.substr(j, 1) != b.seq.substr(k, 1)) {
					overlap = 0;
					break;
				}
				overlap++;
			}
			if (j == a.seq.size() || k == b.seq.size()) {
				readOverlap->setOverlapLen(overlap + 1);
				readOverlap->setReadLen1(a.seq.size());
				readOverlap->setReadLen2(b.seq.size());
				break;
			}
		}
	}
	if (readOverlap->overlapLen == 0) {
		readOverlap->setReadLen1(a.seq.size());
		readOverlap->setReadLen2(b.seq.size());
	}
	return *readOverlap;
}

/**
 * Will prepare an overlap file for a given set of reads.
 * Read1 Id: Read2 Id: Read1 startIdx: Read2 EndIdx: OverlapLength
 */
void prepareOverlapFile(Read* reads, string fileName) {
	long maxOverlap = -1;
	long id1 = 0;
	long id2 = 0;
	int cnt = 0;
	ofstream os;
	os.open(fileName, ofstream::app);
	Read* temps = reads;
	os << "Read1 Id" << " | " << "Read2 Id" << " | " << "Read1 Length" << " | "
			<< "Read2 Length" << " | " << "OverlapLength" << endl;
	while (reads->id != -1) {
		temps = reads;
		while (temps->id != -1) {
			if (temps != reads) {
				ReadOverlap overlap;
				// A and B overlap
				overlap = findOverlap(*reads, *temps);
				os << reads->id << "\t" << temps->id << "\t" << overlap.readLen1
						<< "\t" << overlap.readLen2 << "\t"
						<< overlap.overlapLen << "\n";
				if (maxOverlap < overlap.overlapLen) {
					maxOverlap = overlap.overlapLen;
					id1 = reads->id;
					id2 = temps->id;
				}
				if (overlap.overlapLen > 5) {
					cout << overlap.overlapLen << " " << overlap.readLen1 << " "
							<< overlap.readLen2 << endl;
					cnt++;
				}
				// B and A overlap
				overlap = findOverlap(*temps, *reads);
				os << temps->id << "\t" << reads->id << "\t" << overlap.readLen1
						<< "\t" << overlap.readLen2 << "\t"
						<< overlap.overlapLen << "\n";
				if (maxOverlap < overlap.overlapLen) {
					maxOverlap = overlap.overlapLen;
					id1 = temps->id;
					id2 = reads->id;
				}
				if (overlap.overlapLen > 5) {
					cout << overlap.overlapLen << " " << overlap.readLen1 << " "
							<< overlap.readLen2 << endl;
					cnt++;
				}

				Read revB = findReverseComplement(*temps);
				// A and B" overlap
				overlap = findOverlap(*reads, revB);
				os << reads->id << "\t" << revB.id << "R" << "\t"
						<< overlap.readLen1 << "\t" << overlap.readLen2 << "\t"
						<< overlap.overlapLen << "\n";
				if (maxOverlap < overlap.overlapLen) {
					maxOverlap = overlap.overlapLen;
					id1 = reads->id;
					id2 = revB.id;
				}
				if (overlap.overlapLen > 5) {
					cout << overlap.overlapLen << " " << overlap.readLen1 << " "
							<< overlap.readLen2 << endl;
					cnt++;
				}
				// B" and A overlap
				overlap = findOverlap(revB, *reads);
				os << revB.id << "R" << "\t" << reads->id << "\t"
						<< overlap.readLen1 << "\t" << overlap.readLen2 << "\t"
						<< overlap.overlapLen << "\n";
				if (maxOverlap < overlap.overlapLen) {
					maxOverlap = overlap.overlapLen;
					id1 = revB.id;
					id2 = reads->id;
				}
				if (overlap.overlapLen > 5) {
					cout << overlap.overlapLen << " " << overlap.readLen1 << " "
							<< overlap.readLen2 << endl;
					cnt++;
				}
			}
			temps++;
		}
		reads++;
	}
	cout << maxOverlap << " " << id1 << " " << id2 << " " << cnt << endl;
	os.close();
}

/*
 int main() {
 char file[] = "InputSequences.txt";
 //	char file[] = "NC_000913.100k.pacbio_ec.reads.fasta";
 Read* reads = getReads(file);

 //	while (reads) {
 //		cout << (reads++)->seq;
 //		cout << endl;
 //		reads++;
 //	}
 //	prepareOverlapFile(reads, "OverlapFile.txt");
 //	string str1 = "TCCCAGTTATGTCAGGGGACACGAGCATGCAGAGAC";
 //	string str2 = "AATTGCCGCCGTCGTTTTCAGCAGTTATGTCAGATC";

 Read* temp = reads;
 int len = 0;
 while(temp->id != -1) {
 len++;
 temp++;
 }

 for(int i = 0; i < len; i++) {
 for(int j = i + 1; j < len; j++) {
 Smithwaterman sw(reads[i].seq, reads[j].seq);
 cout << i << ":" << j << ":" << sw.calculateAlignment() << endl;
 }
 }
 //	sw::Smithwaterman sw(str1, str2);
 //	cout << sw.calculateAlignment() << endl;
 return 0;
 }
 */
