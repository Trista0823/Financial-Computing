#pragma once
#include <string>
#include <map>
#include <stdlib.h>
#include <vector>
#include "StockData.h"

using namespace std;

/// This struct used to write data
typedef struct MemoryStruct {
	char* memory;
	size_t size;
};

void* myrealloc(void* ptr, size_t size);

/// This method writes data into file
int write_data(void* ptr, int size, int nmemb, FILE* stream);

/// This method writes data into memory
int write_data2(void* ptr, size_t size, size_t nmemb, void* data);

/// This method calculates the start time and end time for data crawling
string getTimeinSeconds(string Time, int start);

/// This method reads the data from a csv file from Bloomberg
void BloombergData(map<string, StockData>& mapData);

/// This method crawls stock trading data from Yahoo finance
int StockRetrieve(map<string, StockData>& mapData);

/// This method wraps StockRetrieve method and use multi-thread to speed up data retrieve
void multiThread(map<string, StockData>& mapData);

