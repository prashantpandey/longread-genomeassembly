/*
 * ReadFnaFile.h
 *
 * Object to read the fasta file and store the reads in an array
 *  Created on: Nov 6, 2013
 *      Author: prashant
 */

#ifndef READFASTAFILE_H_
#define READFASTAFILE_H_

#include <string>
#include "Read.h"

class ReadFastaFile {
public:
	ReadFastaFile(char* fileName);
	virtual ~ReadFastaFile();

	char* fileName;
	Read* readFastaFile(int *);		// will read the given file and return the content as string

};

#endif /* READFASTAFILE_H_ */
