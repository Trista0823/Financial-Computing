#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <curses.h>
#include <map>
#include "StockData.h"
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <thread>
#include "DataRetrieve.h"
#include "Group.h"
#include "Gnuplot.h"

using namespace std;
typedef vector<float> Vector;
typedef vector<Vector> Matrix;
typedef pair<string, StockData> PAIR;

/// operator overloading for stock information, same overloading functions below
ofstream& operator<<(ofstream& out, StockData& V)
{
    out << V.reportDate << "   " << V.sur << "    " << V.group << endl;
    for (int i =0; i < V.sDate.size();i++)
        out << V.sDate[i] << "   " << V.sReturn[i] << endl;
    return out;
}

/// for stock price
void printresult(StockData &V)
    {
    cout << "Report Date: " << V.reportDate << endl << "Surprise Value: "<<V.sur << endl << "Group:" << V.group_ << endl;
    for (int i =1; i < V.sDate.size();i++)
        cout << V.sDate[i] << "  " << fixed << setprecision(2) << V.sValue[i]<<"  " << 100*V.sReturn[i]<<"%" << endl;
    cout.unsetf(ios::adjustfield|ios::basefield|ios::floatfield|ios::fixed);
    setprecision(4);
    }

/// for stock AR
ostream& operator<<(ostream& out, StockData& V)
{
    out << V.reportDate << "   " << "Group: "<< V.group_ << endl;
    cout << setiosflags(ios::fixed) << setprecision(6);
    for (int i = V.reportIndex-30; i <= V.reportIndex+30; i++)
    out << i-V.reportIndex << "   " << V.sAR[i] << endl;
    cout.unsetf(ios::adjustfield|ios::basefield|ios::floatfield|ios::fixed);
    setprecision(4);
    return out;
}
    
/// for CAAR and AAR vector
ostream& operator<<(ostream& out, Vector& V)
{
    cout << setiosflags(ios::fixed) << setprecision(6);
    for (Vector::iterator itr = V.begin(); itr != V.end(); itr++)
    {
        out << setw(12) << *itr;
        if ((itr - V.begin()) % 4 == 3) cout << endl;
    }
    out << endl << endl;
    cout.unsetf(ios::adjustfield|ios::basefield|ios::floatfield|ios::fixed);
    setprecision(4);
    return out;
}

/// for group composite
ostream& operator<<(ostream& out, vector<string>& V)
    {
        for (vector<string>::iterator itr = V.begin(); itr != V.end(); itr++)
    {
        out << setw(8) << *itr;
        if ((itr - V.begin()) % 6 == 5) cout << endl;
    }
        out << endl << endl;
        return out;
    }

/// for group matrix
ostream& operator<<(ostream& out, Matrix& W)
{
    for (Matrix::iterator itr = W.begin(); itr != W.end(); itr++)
        out << *itr;
    out << endl;
    return out;
}
    

int main()
{
    map<string, StockData> mapData;
    StockData SPY;
    char selection;
    string symbol;
    char choice;
    /// time_flag for data retrieval
    int time_control = 0;
    int index;
    int nIntervals = 60;
    Matrix plotMatrix;
    /// store stock composite
    vector <string> vector_1, vector_2, vector_3;
    int window = 30;
    /// store AAR,CAAR and std
    Matrix AAR(3),CAAR(3),AAR_SD(3),CAAR_SD(3);
    
    /**
    * Using while loop and switch function to show the menu, with five options
    * If user doesn't choose to exit, then the menu would show up repeatedly
    */
    do
    {
        /// menu design
        cout << " =========================================="<<endl;
        cout << "|  C++ Team Project Menu                   |"<<endl;
        cout << "|  ----------------------------------      |"<<endl;
        cout << "|  1.  Retrieve Stock Data                 |"<<endl;
        cout << "|  2.  Show Stock Historical Data          |"<<endl;
        cout << "|  3.  Show AAR/CAAR of Each Group         |"<<endl;
        cout << "|  4.  Plot Graph of CAAR                  |"<<endl;
        cout << "|  5.  Exit                                |"<<endl;;
        cout << " =========================================="<<endl;
        cout << "  Enter Menu Number: ";
        cin >> selection;
        cout << endl;

        switch (selection)
        {
        case '1':
            /**
            * Read stock list from csv file and get data using libcurl
            * store stock data into stock structure and finish bootstrapping
            */
            if (time_control == 0){
                BloombergData(mapData);
                mapData["SPY"] = SPY;
                cout <<"Bloomberg csv Read\n";
                multiThread(mapData);

                Group group(mapData);

                group.Beat->BootStrap(window);
                AAR[0] = group.Beat->CalStd(window)[0];
                AAR_SD[0] = group.Beat->CalStd(window)[1];
                CAAR[0] = group.Beat->CalStd(window)[2];
                CAAR_SD[0] = group.Beat->CalStd(window)[3];
                vector_1 = group.Beat->GetNameVector();
    
                group.Meet->BootStrap(window);
                AAR[1] = group.Meet->CalStd(window)[0];
                AAR_SD[1] = group.Meet->CalStd(window)[1];
                CAAR[1] = group.Meet->CalStd(window)[2];
                CAAR_SD[1] = group.Meet->CalStd(window)[3];
                vector_2 = group.Meet->GetNameVector();
    
                group.Miss->BootStrap(window);
                AAR[2] = group.Miss->CalStd(window)[0];
                AAR_SD[2] = group.Miss->CalStd(window)[1];
                CAAR[2] = group.Miss->CalStd(window)[2];
                CAAR_SD[2] = group.Miss->CalStd(window)[3];
                vector_3 = group.Miss->GetNameVector();
                
                /// forbid repetitve retrieval
                time_control += 1;
                cout << "Data Retrieved"<<endl<<endl;
                break;}
            else{
                cout << "Data Already Retrieved" <<endl;
                cout << "Choose Other Menu Options"<<endl<<endl;
                break;
                }

        case '2':
                /**
                * Print stock price data or AR data
                * Print group composite
                */
                cout << "Stock Data[Y] or Group Composite[N]:\n";
                cin >> choice;
                if(choice == 'Y' || choice =='y')
    {
                cout << "Price Data[Y] or AR Data[N]:\n";
                cin >> choice;
                cout << "Enter Stock Symbol:";
                cin >> symbol;
                transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);
                if (choice =='Y' || choice =='y')
                    {
                    printresult(mapData[symbol]);
                    }
                else if (choice =='N' || choice =='n')
                    {
                    cout << mapData[symbol];
                    }
    }
                else if(choice =='N' || choice =='n')
            {
                cout << "| ======================================== |"<<endl;
                cout << "|  1.  Show Beat Group                     |"<<endl;
                cout << "|  2.  Show Meet Group                     |"<<endl;
                cout << "|  3.  Show Miss Group                     |"<<endl;
                cout << "| ======================================== |"<<endl;
                cout << "  Enter Menu Number: ";
                cin >> choice;
                if (choice == '1')
                    {
                        cout << "Beat Group Composite: \n" << vector_1 <<endl;
                    }
                else if (choice == '2')
                    {
                        cout << "Meet Group Composite: \n" << vector_2 <<endl;
                    }
                else if (choice == '3')
                    {
                        cout << "Miss Group Composite: \n" << vector_3 <<endl;
                    }
            }
                break;

        case '3':
                /// Show AAR and CAAR with std
                cout << "| ======================================== |"<<endl;
                cout << "|  1.  Show AAR                            |"<<endl;
                cout << "|  2.  Show CAAR                           |"<<endl;
                cout << "| ======================================== |"<<endl;
                cout << "  Enter Menu Number: ";
                cin>>index;
                if(index == 1)
                {
                    cout << "Beat group_AAR:\n" << AAR[0]<< endl;
                    cout << "Beat group_AAR_SD:\n" << AAR_SD[0]<< endl;
                    cout << "Meet group_AAR:\n" << AAR[1]<< endl;
                    cout << "Meet group_AAR_SD:\n" << AAR_SD[1]<< endl;
                    cout << "Miss group_AAR:\n" << AAR[2] << endl;
                    cout << "Miss group_AAR_SD:\n" << AAR_SD[2] << endl;
                    break;
                }
                else if(index == 2)
                {
                    cout << "Beat group_CAAR:\n" << CAAR[0]<< endl;
                    cout << "Beat group_CAAR_SD:\n" << CAAR_SD[0]<< endl;
                    cout << "Meet group_CAAR:\n" << CAAR[1]<< endl;
                    cout << "Meet group_CAAR_SD:\n" << CAAR_SD[1]<< endl;
                    cout << "Miss group_CAAR:\n" << CAAR[2] << endl;
                    cout << "Miss group_CAAR_SD:\n" << CAAR_SD[2] << endl;
                    break;
                }
                else{
                    cout <<"Please Enter Correct Number Again: \n"<<endl;
                    cin>>index;
                    if(index == 1)
                    {
                        cout << "Beat group:" << AAR[0]<< endl<<endl;
                        cout << "Meet group:" << AAR[1]<< endl<<endl;
                        cout << "Miss group:" << AAR[2] << endl<<endl;
                        break;
                    }
                    else if(index == 2)
                    {
                        cout << "Beat group:" << CAAR[0] << endl<<endl;
                        cout << "Meet group:" << CAAR[1]<< endl<<endl;
                        cout << "Miss group:" << CAAR[2] << endl<<endl;
                        break;
                    }
                    }
                break;
    
        case '4':
                /// plot CAAR for each group
                plotMatrix.push_back(CAAR[0]);
                plotMatrix.push_back(CAAR[1]);
                plotMatrix.push_back(CAAR[2]);
                cout << "Press Enter to Continue...";
                plotresults(plotMatrix,nIntervals);
                break;

        case '5':
                /// exit the program
                cout << "Exit the Program"<<endl;
                cout << "Presented by Mengyao Bao, Duanyang Zhang & Dingtian Zhu"<<endl;
                return 0;

        default:
                /// back to menu
                cout << selection << " is not a valid selection.\n";
                cout << endl;
        }
    } while (selection!= 0);
    
    return 0;
}
    
