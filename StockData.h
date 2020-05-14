using namespace std;
#include <vector>
#include <string>

#ifndef StockData_h
#define StockData_h

/// This struct is used to store all the information for one stock
typedef struct STOCKDATA {
    vector<string> sDate;        ///< trading dates from Yahoo finance around the report date
    vector<float> sValue;        ///< adjusted stock value corresponding to trading dates
    vector<float> sReturn;        ///< stock return corresponding to trading dates
    vector<float> sAR;            ///< stock abnormal return corresponding to trading dates
    string reportDate;            ///< report date from Bloomberg
    int reportIndex;            ///< the index for report date in the sDate vector
    float sur;                    ///< earning surprise calculated from Bloomberg data
    int group;                    ///< 1 for beat group; 0 for meet group; -1 for miss group
    string group_;                ///< group name:["Beat", "Meet", "Miss"]
}StockData;

#endif // !StockData_h

