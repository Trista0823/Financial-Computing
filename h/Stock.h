#pragma once
#include <map>
#include "StockData.h"
#include <string>
#include <vector>

typedef vector<float> Vector;
typedef vector<Vector> Matrix;
typedef pair<string, StockData> PAIR;

class Stock
{
private:
    Matrix PriceMatrix; /// stock price of whole group
    vector<string> NameVector; /// for stock name of whole group
public:
    Stock(Matrix PriceMatrix_, vector<string> NameVector_) { PriceMatrix = PriceMatrix_; NameVector = NameVector_; } /// Constructor with paramerter
    vector<Matrix> BootStrap(int window);  /// Computing AAR and CAAR, return a vector, vector[0] -> AAR and vector[1] -> CAAR
    vector<Vector> CalStd(int window); /// Function for calculating standard deviation
    vector<string> GetNameVector() { return NameVector; } /// Get the name list of whole group
    Matrix GetPriceMatrix() { return PriceMatrix; } /// Get the stock price list of whole group
};
