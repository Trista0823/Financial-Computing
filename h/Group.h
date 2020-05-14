#pragma once
#include "Stock.h"
#include <map>
#include "StockData.h"
#include <string>
#include <vector>


typedef vector<float> Vector;
typedef vector<Vector> Matrix;
typedef pair<string, StockData> PAIR;

class Group
{
public:
    /// Group have three pointers to beat group, meet group and miss group object of Stock respectly
	Stock* Beat;
	Stock* Meet;
	Stock* Miss;
    /// constructor with parameters, will retrieve the data and generate three stock objects by dynamic allocation
	Group(map<string, StockData> & mapdata);
    /// free the memory of dynamic allocation
	~Group() { delete Beat; delete Meet; delete Miss;}
};

