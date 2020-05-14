

#ifndef gnuplot_h
#define gnuplot_h
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <iostream>
using namespace std;

/// use gnulot to plot CAAR matrix for each group
typedef vector<vector<float>> Matrix;
void plotresults(Matrix C,int size_)
{
    ofstream fout;
    /// set matrix data into CAAR.txt
    fout.open("CAAR.txt");
    for (int i = 0; i <= size_; i++)
    {
        fout << i - 30 << " " << C[0][i] << " " << C[1][i] << " " << C[2][i] << endl;
    }
    fout.close();
    
    fout.open("Plot.txt");
    /// set title, legend and line pattern, then show the plot
    fout << "set title \"CAAR\"" << endl;
    fout << "plot 'CAAR.txt' using 1:3 title 'Meet' with lines, 'CAAR.txt' using 1:2 title 'Beat' with lines, 'CAAR.txt' using 1:4 title 'Miss' with lines" << endl;
    fout << "pause -1" << endl;
    fout.close();
    system("/opt/local/bin/gnuplot Plot.txt");
}

#endif /* gnuplot_h */
