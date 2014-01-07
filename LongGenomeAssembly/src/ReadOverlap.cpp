/*
 * ReadOverlap.cpp
 *
 *  Created on: Nov 9, 2013
 *      Author: prashant
 */

#include "ReadOverlap.h"

namespace ro {

ReadOverlap::ReadOverlap() {
	ReadOverlap::overlapLen = 0;
	ReadOverlap::readLen1 = 0;
	ReadOverlap::readLen2 = 0;
}

ReadOverlap::~ReadOverlap() {
	// TODO Auto-generated destructor stub
}

long ReadOverlap::getOverlapLen() {
	return ReadOverlap::overlapLen;
}

void ReadOverlap::setOverlapLen(long overlapLen) {
	ReadOverlap::overlapLen = overlapLen;
}

long ReadOverlap::getReadLen1() {
	return ReadOverlap::readLen1;
}

void ReadOverlap::setReadLen1(long readLen1) {
	ReadOverlap::readLen1 = readLen1;
}

long ReadOverlap::getReadLen2() {
	return ReadOverlap::readLen2;
}

void ReadOverlap::setReadLen2(long readLen2) {
	ReadOverlap::readLen2 = readLen2;
}

} /* namespace ro */
