/*
 * HashFreq.h
 *
 *  Created on: Nov 12, 2013
 *      Author: prashant
 */

#ifndef HASHFREQ_H_
#define HASHFREQ_H_

namespace hf {

class HashFreq {
public:
	HashFreq(long hash, long freq);
	virtual ~HashFreq();

	long hash;
	long freq;

	void sethash(long hash);
	void setFreq(long freq);

	long getHash();
	long getFreq();

	void incrementFreq(HashFreq hash);
	bool compare(HashFreq hash1, HashFreq freq2);


};

} /* namespace sw */

#endif /* HASHFREQ_H_ */
