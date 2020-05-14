#include "Stock.h"
#include <math.h>


Matrix matrix_transpose(const Matrix& matrix)
{
    Matrix matrixT(matrix[0].size());

    for (int i = 0; i < matrix[0].size(); ++i)
    {
        matrixT[i].resize(matrix.size());
        for (int j = 0; j < matrix.size(); ++j)
        {
            matrixT[i][j] = matrix[j][i];
        }
    }
    return matrixT;
}

float calculateSD(const Vector& data)
{
    float sum = 0.0, mean, standardDeviation = 0.0;

    int i;

    for (i = 0; i < data.size(); ++i)
    {
        sum += data[i];
    }
    mean = sum / (i+1);
    for (i = 0; i < data.size(); ++i)
        standardDeviation += pow(data[i] - mean, 2);
    return sqrt(standardDeviation / data.size());
}

float calculateMean(const Vector& data)
{
    float mean,sum = 0.0;

    int i;

    for (i = 0; i < data.size(); ++i)
    {
        sum += data[i];
    }
    mean = sum / (i+1);

    return mean;
}

/// Generate a series of different random integers from [0,n)
void getRands(vector<int>& L, int n)
{
    int i = 0, temp;
    while (i < L.size())
    {
        temp = (rand() % (n - 1 + 0 + 1)) + 0;
        auto it = find(L.begin(), L.end(), temp);
        if (it == L.end())
        {
            L[i] = temp;
            i++;
        }
        else
            continue;
    }
}

/** \window: number of days before or after the report date. 30 is the possible maximum value
  * \return: a vector of matrix, the first element is the AAR and the second is the CAAR
  *
  * For each group, conduct a bootstrap. Store the average abnormal return of selected stocks for each sampling in the AAR_group,
  * the cumulative abnormal return of selected stocks for each sampling in the CAAR_group.
  */
vector<Matrix> Stock::BootStrap(int window)
{
    vector<int> rands(30,-1);                    ///<random select 30 companies
    int k = 30;                                    ///< bootstrap times
    vector<Matrix> result(2);
    Matrix AAR_group(k);
    Matrix CAAR_group(k);
    for (int i = 0; i < k; i++)
    {
        AAR_group[i].resize(2 * window + 1);
        fill(AAR_group[i].begin(), AAR_group[i].end(), 0);
        CAAR_group[i].resize(2 * window + 1);
        fill(CAAR_group[i].begin(), CAAR_group[i].end(), 0);
    }
    for (int n = 0; n < k; n++)
    {
        getRands(rands, PriceMatrix.size());
        for (int t = 0; t <= 2 * window; t++)
        {
            for(int i =0; i<rands.size();i++)
            {
                AAR_group[n][t] +=  (PriceMatrix[rands[i]][t] / rands.size());
            }
            if (t == 0) {CAAR_group[n][t] = AAR_group[n][t];}
            else  {CAAR_group[n][t] = CAAR_group[n][t-1]+AAR_group[n][t];}
        }
    }
    result[0] = AAR_group;
    result[1] = CAAR_group;
    return result;
}

/** \calculate the mean and standard deviation for the result of bootstrap
  * \window: number of days before or after the report date. 30 is the possible maximum value
  * \return: vector of vector, contains the mean and standard devidation of AAR or CAAR from bootstrap
  *
  * Since each row in the result of bootstrap is the average abnormal return or cumulative abnormal return among different days for one
  * sampling, however, we need to calculate the average for one day among different sampling. We need to transpose the matrix first.
  */
vector<Vector> Stock::CalStd(int window) {
    vector<Matrix> ACRR = BootStrap(window);
    vector<Vector> result(4);
    Vector AAAR_group;
    Vector stdAAR_group;
    Vector ACAAR_group;
    Vector stdCAAR_group;
    Matrix AAR_group = matrix_transpose(ACRR[0]);
    Matrix CAAR_group = matrix_transpose(ACRR[1]);
    for (int i = 0; i <= 2 * window; i++)
    {
        stdAAR_group.push_back(calculateSD(AAR_group[i]));
        AAAR_group.push_back(calculateMean(AAR_group[i]));
        stdCAAR_group.push_back(calculateSD(CAAR_group[i]));
        ACAAR_group.push_back(calculateMean(CAAR_group[i]));
    }
    result[0] = AAAR_group;
    result[1] = stdAAR_group;
    result[2] = ACAAR_group;
    result[3] = stdCAAR_group;
    return result;
}
