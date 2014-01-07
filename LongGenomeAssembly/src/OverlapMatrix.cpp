/*
 * OverlapMatrix.cpp
 *
 *  Created on: Dec 6, 2013
 *      Author: prashant
 */

#include <sstream>

#include "OverlapMatrix.h"

namespace sw {

enum orientation {
	N, S
};

OverlapMatrix::OverlapMatrix() {
	OverlapMatrix::readId1 = 0;
	OverlapMatrix::readId2 = 0;
	OverlapMatrix::orientation = ' ';
	OverlapMatrix::ahg = 0;
	OverlapMatrix::bhg = 0;
	OverlapMatrix::score = 0;
}

OverlapMatrix::~OverlapMatrix() {
	// TODO Auto-generated destructor stub
}

OverlapMatrix::OverlapMatrix(int id1, int id2, char o, int ahg, int bhg,
		int score) {
	OverlapMatrix::readId1 = id1;
	OverlapMatrix::readId2 = id2;
	OverlapMatrix::orientation = o;
	OverlapMatrix::ahg = ahg;
	OverlapMatrix::bhg = bhg;
	OverlapMatrix::score = score;
}

void OverlapMatrix::setReadId1(int id) {
	OverlapMatrix::readId1 = id;
}
void OverlapMatrix::setReadId2(int id) {
	OverlapMatrix::readId2 = id;
}
void OverlapMatrix::setOrientation(char o) {
	OverlapMatrix::orientation = o;
}
void OverlapMatrix::setAhg(int ahg) {
	OverlapMatrix::ahg = ahg;
}
void OverlapMatrix::setBhg(int bhg) {
	OverlapMatrix::bhg = bhg;
}
void OverlapMatrix::setScore(int score) {
	OverlapMatrix::score = score;
}

int OverlapMatrix::getReadId1() {
	return OverlapMatrix::readId1;
}
int OverlapMatrix::getReadId2() {
	return OverlapMatrix::readId2;
}

int OverlapMatrix::getAhg() {
	return OverlapMatrix::ahg;
}
int OverlapMatrix::getBhg() {
	return OverlapMatrix::bhg;
}
int OverlapMatrix::getScore() {
	return OverlapMatrix::score;
}

string OverlapMatrix::toString() {
	stringstream ss;
	ss << "{ OVL\nOrn: " << OverlapMatrix::orientation << "\nReads: "
			<< OverlapMatrix::readId1 << "," << OverlapMatrix::readId2
			<< "\nScore: " << OverlapMatrix::score << "\nahg: "
			<< OverlapMatrix::ahg << "\nbhg: " << OverlapMatrix::bhg << "\n}";
	return ss.str();
}

} /* namespace sw */
