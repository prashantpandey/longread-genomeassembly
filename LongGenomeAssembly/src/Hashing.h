/*
 * Hashing.h
 *
 *  Created on: Dec 1, 2013
 *      Author: prashant
 */

#ifndef HASHING_H_
#define HASHING_H_

#include <iostream>

using namespace std;

namespace lcr {

class Hashing {
public:
	Hashing();
	virtual ~Hashing();

	/**
	 * Will return the ASCII value of the char
	 */
	static int returnAscii(char c) {
		//	for (int i = 0; i < strlen(bases); i++) {
		//		if (bases[i] == c)
		//			return i + 1;
		//	}
		switch (c) {
		case 'A':
			return 27;
		case 'T':
			return 46;
		case 'C':
			return 29;
		case 'G':
			return 33;
		default:
			break;
		}
		return -1;
	}

	/**
	 * Power function
	 */
	static int power(long base, long modulo, int power) {
		int result = 1;
		for (int i = 1; i <= power; i++) {
			result = (int) ((result * base) % modulo);
		}
		return result;
	}

	static int calculateHash(string pattern, long base, long modulo) {
		int hashValue = 0;
		int j = 0;
		for (int i = pattern.size() - 1; i >= 0; i--) {
//		cout << pattern[i] << ":" << power(base, modulo, j) << ":" << returnAscii(pattern[i]) << ":" << hashValue <<  endl;

			hashValue += ((returnAscii(pattern[i]) * power(base, modulo, j)));
			j++;
		}
		return (hashValue % modulo);
	}

	static int calculateRollingHash(char oldChar, char newChar, int oldHash,
			int kmer, long base, long modulo) {
		return ((((base * oldHash) + returnAscii(newChar)
				- power(base, modulo, kmer) * returnAscii(oldChar)) % modulo
				+ modulo) % modulo);
	}

	static int calculateSpacedSeedHash(string pattern, long base, long modulo, int* seed) {
		int hashValue = 0;
		int j = 0;
		for (int i = pattern.size() - 1; i >= 0; i--) {
//		cout << pattern[i] << ":" << power(base, modulo, j) << ":" << returnAscii(pattern[i]) << ":" << hashValue <<  endl;

			hashValue += ((returnAscii(pattern[i]) * power(base, modulo, j)) * seed[i]);
			j++;
		}
		return (hashValue % modulo);
	}

};
} /* namespace lcr */

#endif /* HASHING_H_ */
