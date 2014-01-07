/*
 * HashFreq.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: prashant
 */

#include "HashFreq.h"

namespace hf {

HashFreq::HashFreq(long hash, long freq) {
	HashFreq::hash = hash;
	HashFreq::freq = freq;
}

HashFreq::~HashFreq() {
	// TODO Auto-generated destructor stub
}

void HashFreq::sethash(long hash) {
	HashFreq::hash = hash;
}

void HashFreq::setFreq(long freq){
	HashFreq::freq = freq;
}

long HashFreq::getHash() {
	return HashFreq::hash;
}

long HashFreq::getFreq() {
	return HashFreq::freq;
}

void HashFreq::incrementFreq(HashFreq hash){
	HashFreq::freq++;
}

bool HashFreq::compare(HashFreq hash1, HashFreq hash2) {
	if(hash1.hash == hash2.hash) {
		return true;
	}
	return false;
}

} /* namespace hf */
