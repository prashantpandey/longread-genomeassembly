/*
 * ReadFastaFile.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: prashant
 */

#include "ReadFastaFile.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include "Read.h"

using namespace std;

ReadFastaFile::ReadFastaFile(char* file) {
	fileName = file;
}

ReadFastaFile::~ReadFastaFile() {

}

/**
 * To split the string based upon a token
 */
vector<string> split(const string& s, const string& delim,
		const bool keep_empty = true) {
	vector<string> result;
	if (delim.empty()) {
		result.push_back(s);
		return result;
	}
	string::const_iterator substart = s.begin(), subend;
	while (true) {
		subend = search(substart, s.end(), delim.begin(), delim.end());
		string temp(substart, subend);
		if (keep_empty || !temp.empty()) {
			result.push_back(temp);
		}
		if (subend == s.end()) {
			break;
		}
		substart = subend + delim.size();
	}
	return result;
}

/**
 * To read the fasta file and prepare Read data structure
 */
Read* ReadFastaFile::readFastaFile(int* numReads) {
	ifstream file(ReadFastaFile::fileName);
	Read* reads = new Read[12000];
	if (file) {
		string line;
		long i = 1;
		while (getline(file, line)) {
			vector<string> split1 = split(line, ":");
//			copy(split1.begin(), split1.end(), ostream_iterator<string>(cout, "\n"));

			vector<string> split2 = split(split1[1], "-");
//			copy(split2.begin(), split2.end(), ostream_iterator<string>(cout, "\n"));

			reads[i].setId(i);
			reads[i].setStart((long) stol(split2[0], NULL, 10));
			reads[i].setEnd((long) stol(split2[1], NULL, 10));
			reads[i].setLength(
					(long) stol(split2[1], NULL, 10)
							- (long) stol(split2[0], NULL, 10));

			getline(file, line);
			reads[i].setSeq(line);
//			cout << reads[i].start << ":" << reads[i].end << ":" << reads[i].length << endl;

			i++;
		}
		reads[i].setId(-1);
//		cout << i << endl;				// number of reads
		*numReads = i;
	}
	file.close();
	return reads;
}
