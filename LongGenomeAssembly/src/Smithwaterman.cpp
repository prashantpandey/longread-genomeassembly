/*
 * Smithwaterman.cpp
 *
 *  Created on: Nov 11, 2013
 *      Author: prashant
 */

#include <string>
#include <iostream>
#include <fstream>

#include "Smithwaterman.h"
#include "Cell.h"
#include "CellIdx.h"

using namespace std;

namespace sw {

Smithwaterman::Smithwaterman(string str1, string str2) {
	Smithwaterman::str1 = str1;
	Smithwaterman::str2 = str2;
	Smithwaterman::startCell = CellIdx();
	Smithwaterman::goalCell = CellIdx();
	Smithwaterman::cell = Smithwaterman::costMatrixInit(str1.size(),
			str2.size());
}

Smithwaterman::~Smithwaterman() {
	// TODO Auto-generated destructor stub
}

/**
 *
 * @param c
 * @param d
 * @return
 */
int match(char c, char d) {
	if (c == d)
		return 5;
	return -4;
}

/**
 *
 * @param c
 * @return
 */
int indel(char c) {
	return -4;
}

/**
 * To print the given matrix
 * @param i
 * @param j
 */
void Smithwaterman::matchOut(int i, int j) {
	if (Smithwaterman::str1.substr(i, 1) == Smithwaterman::str2.substr(j, 1))
		cout << Smithwaterman::str1.substr(i, 1);
}

/**
 * To print the given matrix
 * @param j
 */
void Smithwaterman::insertOut(int j) {
	cout << Smithwaterman::str2.substr(j, 1);
}

/**
 * To print the given matrix
 * @param i
 */
void Smithwaterman::deleteOut(int i) {
	cout << Smithwaterman::str1.substr(i, 1);
}

/**
 *
 * @param len1
 * @param len2
 */
Cell** Smithwaterman::costMatrixInit(int len1, int len2) {
	Cell** cell = new Cell*[len1 + 1];
	for (int i = 0; i <= len1; i++) {
		cell[i] = new Cell[len2 + 1];
	}
	for (int i = 0; i <= len1; i++) {
		for (int j = 0; j <= len2; j++) {
			cell[i][j].setCost(0);
			cell[i][j].setParent(-1);
		}
	}
	return cell;
}

void Smithwaterman::reconstructPath(int i, int j) {
	if (cell[i][j].parent == -1) {
		startCell.row = i;
		startCell.column = j;
		return;
	}
	if (cell[i][j].parent == MATCH) {
		reconstructPath(i - 1, j - 1);
//		matchOut(i, j);
		return;
	}
	if (cell[i][j].parent == INSERT) {
		reconstructPath(i, j - 1);
//		insertOut(j);
		return;
	}
	if (cell[i][j].parent == DELETE) {
		reconstructPath(i - 1, j);
//		deleteOut(i);
		return;
	}
}

CellIdx Smithwaterman::getGoalCell(int len1, int len2) {
	for (int i = 0; i < len1; i++) {
		for (int j = 0; j < len2; j++) {
			if (cell[i][j].cost > cell[goalCell.row][goalCell.column].cost) {
				goalCell.row = i;
				goalCell.column = j;
			}
		}
	}
//	cout << endl;
	reconstructPath(goalCell.row, goalCell.column);
//	cout << endl;
	return goalCell;
}

int Smithwaterman::calculateAlignment() {
	int opt[3];
	int len1 = str1.size();
	int len2 = str2.size();

	for (int i = 1; i < len1; i++) {
		for (int j = 1; j < len2; j++) {

			// Normal Smith-Waterman Algorithm
			opt[MATCH] = cell[i - 1][j - 1].cost + match(str1[i], str2[j]);
			opt[INSERT] = cell[i][j - 1].cost + indel(str2[j]);
			opt[DELETE] = cell[i - 1][j].cost + indel(str1[i]);

			cell[i][j].cost = 0;
			cell[i][j].parent = -1;
			for (int k = MATCH; k < DELETE; k++) {
				if (opt[k] > cell[i][j].cost) {
					cell[i][j].cost = opt[k];
					cell[i][j].parent = k;
				}
			}
		}
	}

	// printMatrix(len1, len2);
	CellIdx finalCell = getGoalCell(len1, len2);
//	cout << "\n";
	return cell[finalCell.row][finalCell.column].cost;
}

void Smithwaterman::printMatrix(int len1, int len2) {
	for (int i = 0; i < len1; i++) {
		for (int j = 0; j < len2; j++) {
			cout << cell[i][j].cost << " ";
		}
		cout << "\n";
	}
}

OverlapMatrix Smithwaterman::prepareOverlapMatrix(int id1, int len1, int id2,
		int len2) {
	int score = calculateAlignment();
//	cout << "start cell: " << startCell.row << ":" << startCell.column << endl;
//	cout << "end cell: " << goalCell.row << ":" << goalCell.column << endl;
	int ahg = startCell.row - startCell.column;
	int bhg = len2 - goalCell.column - (len1 - goalCell.row);
	char ornt = ' ';
	if (ahg == 0 && bhg == 0) {
		ornt = 'C';
	}
	if (ahg >= 0 && bhg >= 0) {
		ornt = 'N';
	} else if (ahg <= 0 && bhg <= 0) {
		ornt = 'S';
	} else {
		ornt = 'C';
	}
	OverlapMatrix overlap(id1, id2, ornt, ahg, bhg, score);
	return overlap;
}

//int main(int argc, char **argv) {
//	string str1 = "TCCCAGTTATGTCAGGGGACACGAGCATGCAGAGAC";
//	string str2 = "AATTGCCGCCGTCGTTTTCAGCAGTTATGTCAGATC";
//	Smithwaterman sw(str1, str2);
//	cout << sw.calculateAlignment() << endl;
//	return 0;
//}

} /* namespace sw */
