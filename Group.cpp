#include "Group.h"
#include <iostream>
#include <algorithm>
using namespace std;

/// This method is used for taking a subvector from a whole vector with a known start index and a known end index
template <typename T> vector<T> slice(const vector<T>& vect, int start, int end)
{
    vector<T> ans;
    for (int i = start; i <= end; i++)
    {
        ans.push_back(vect[i]);
    }
    return ans;
}

/// This method is used for sorting by earning surprise.
int cmp(const PAIR& x, const PAIR& y)
{
    return x.second.sur > y.second.sur;
}


Group::Group(map<string, StockData> & mapData) {

    StockData SPY;
    auto it = mapData.find("SPY");
    SPY = it->second;
    mapData.erase(it);


    /// Calculate return of SPY
    SPY.sReturn.resize(SPY.sDate.size());
    for (int i = 1; i < SPY.sDate.size(); i++)
    {
        SPY.sReturn[i] = (SPY.sValue[i] - SPY.sValue[i - 1]) / SPY.sValue[i - 1];
    }


    /// Calculate return, abnormal return and reportIndex for stock in mapData
    map<string, StockData>::iterator miter;
    for (miter = mapData.begin(); miter != mapData.end(); miter++)
    {
        (miter->second).sReturn.resize((miter->second).sDate.size());
        (miter->second).sAR.resize((miter->second).sDate.size());
        /// find the first trading date of the stock from the SPY trading dates
        auto it = find(SPY.sDate.begin(), SPY.sDate.end(), (miter->second).sDate[1]);
        /// calculate the index of the first trading date of the stock in the SPY trading dates
        string reportDate = (miter->second).reportDate;
        /// find the report date from the trading dates of the stock
        auto mit = find((miter->second).sDate.begin(), (miter->second).sDate.end(), (miter->second).reportDate);

        /**
        * There was a stock release quartly report on non-trading day. Then we change the report date to the first
        * trading day after the report date in the csv file from Bloomberg.
        */
        if (mit == (miter->second).sDate.end())
        {
            cout << "error" << endl;
            cout << (miter->first) << endl;
        }
        if (it != SPY.sDate.end())
        {
            auto index = distance(SPY.sDate.begin(), it);
            for (int i = 1; i < (miter->second).sDate.size(); i++)
            {
                (miter->second).sReturn[i] = ((miter->second).sValue[i] - (miter->second).sValue[i - 1]) / (miter->second).sValue[i - 1];
                (miter->second).sAR[i] = (miter->second).sReturn[i] - SPY.sReturn[index];
                index++;
            }
            (miter->second).reportIndex = distance((miter->second).sDate.begin(), mit);
        }
        else
            cout << "Failed to find corresponding date." << endl;
    }


    /**
    * Convert the map structure into a vector structure and sort the vector by the earning surprise,
    * which is defined in the cmp method. Then use the index to fill group information of the map
    */
    vector<PAIR> vec(mapData.begin(), mapData.end());
    sort(vec.begin(), vec.end(), cmp);

    int n = vec.size();
    for (size_t i = 0; i != vec.size(); ++i)
    {
        ///beat group
        if (i <= n * 0.3)
        {
            mapData[vec[i].first].group = 1;
            mapData[vec[i].first].group_ = "Beat";
        }
        
        ///miss group
        else if (i >= n * 0.7)
        {
            mapData[vec[i].first].group = -1;
            mapData[vec[i].first].group_ = "Miss";
        }

        ///meet group
        else
        {
            mapData[vec[i].first].group = 0;
            mapData[vec[i].first].group_ = "Meet";
        }
        
    }
    
    /**
    * Use 3 matrixs to store abnormal return during [reporteDate - 30, reportDate + 30] for each group;
    * Use 3 vectors to store indexs of stocks in each group.
     */

    Matrix beat;
    Matrix meet;
    Matrix miss;
    vector<string> beat_name;
    vector<string> meet_name;
    vector<string> miss_name;
    int window = 30;
    for (miter = mapData.begin(); miter != mapData.end(); miter++)
    {
        Vector tempAR;
        switch ((miter->second).group)
        {
        case -1:
            tempAR = slice((miter->second).sAR, (miter->second).reportIndex - window, (miter->second).reportIndex + window);
            miss.push_back(tempAR);
            miss_name.push_back(miter->first);
            break;
        case 0:
            tempAR = slice((miter->second).sAR, (miter->second).reportIndex - window, (miter->second).reportIndex + window);
            meet.push_back(tempAR);
            meet_name.push_back(miter->first);
            break;
        case 1:
            tempAR = slice((miter->second).sAR, (miter->second).reportIndex - window, (miter->second).reportIndex + window);
            beat.push_back(tempAR);
            beat_name.push_back(miter->first);
            break;
        }
    }
    Beat = new Stock(beat, beat_name);
    Miss = new Stock(miss, miss_name);
    Meet = new Stock(meet, meet_name);
}

