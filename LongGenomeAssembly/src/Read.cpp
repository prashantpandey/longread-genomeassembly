/*
 * Read.cpp
 *
 *  Created on: Nov 9, 2013
 *      Author: prashant
 */

#include "Read.h"
#include <string>

using namespace std;

Read::Read() {
	Read::id = 0;
	Read::seq = "";
	Read::length = 0;
	Read::start = 0;
	Read::end = 0;
}

Read::~Read() {
	// TODO Auto-generated destructor stub
}

int Read::getId() {
	return Read::id;
}

string Read::getSeq() {
	return Read::seq;
}

void Read::setId(int id) {
	Read::id = id;
}

void Read::setSeq(string seq) {
	Read::seq = seq;
}

long Read::getLength() {
	return Read::length;
}

long Read::getStart() {
	return Read::start;
}

long Read::getEnd() {
	return Read::end;
}

void Read::setLength(long length) {
	Read::length = length;
}

void Read::setStart(long start) {
	Read::start = start;
}

void Read::setEnd(long end) {
	Read::end = end;
}

/**
 * Will return the complementary base pair
 */
char complementBp(char a) {
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
 * Will return the reverse complement of the current sequence
 */
string Read::getReverseComplement() {
	string revSeq;
	for (int i = seq.length() - 1; i >= 0; i--) {
		revSeq.append(1, complementBp(seq[i]));
	}
	return revSeq;
}

