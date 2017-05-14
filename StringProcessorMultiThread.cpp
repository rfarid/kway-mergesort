// String processor (single-threaded)
// The base of this code is kway-mergesort from https://github.com/arq5x/kway-mergesort
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>

using namespace std;

// local includes
#include "kwaymergesort_multi_thread.h"
int  bufferSize     = 100000;      // allow the sorter to use 100Kb (base 10) of memory for sorting.  
                                   // once full, it will dump to a temp file and grab another chunk.     
char outFile[]       = "out_sorted.txt";
// comparison functions for the ext. merge sort	
bool alphaAsc(const string &a, const string &b) { return a < b; }

int main(int argc, char* argv[]) {
    int delaySec        = atoi(argv[1]);
    string inFile       = argv[2];
    bool compressOutput = false;       // not yet supported
    string tempPath     = "./";        // allows you to write the intermediate files anywhere you want.
    ofstream outputFile(outFile);
    // sort the lines of a file lexicographically in ascending order (akin to UNIX sort, "sort FILE")
    KwayMergeSort<string> *sorter = new KwayMergeSort<string> (inFile, 
                                                               delaySec,      
                                                               &outputFile, 
                                                               alphaAsc, 
                                                               bufferSize, 
                                                               compressOutput, 
                                                               tempPath);
    sorter->Sort();
}
