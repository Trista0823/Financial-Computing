# Financial_Computing

## Programming Requirements:
- Use liburl to retrieve historical price data from Yahoo Finance: A function retrieves the adjusted close prices for selected S & P 500 stocks and SPY into memory.
- Create a set of classes such as class for stock to handle EPS estimate and price information.
- Use member functions or independent functions for all calculation. Overload a few arithmetic operators for vector/matrix.
- The stocks and their corresponding price information for each group should be stored in a STL map, with stock symbol as its keys.
- The expected AAR, AAR SD, and expected CAAR and CAAR SD for 3 groups are presented in a matrix. The row of the matrix is the group#, matrix columns are for AAR, AAR-SD, CAAR, CAAR-SD
- Use gnuplot(MAC) to show the CAAR from all 3 groups in one graph.              
 <br/>  
 
## Functions:
- Retrieve historical price data for all selected stocks. Parse the retrieved data for dates and adjusted closing prices.
- Calculate AAR, AAR-SD, CAAR CAAR-SD for each group
- Populate the stock maps and AAR/CAAR matrix.
-  Show an Excel or gnuplot graph with CAAR for all 3 groups.       
 <br/> 
 
## Menu:
- Retrieve historical price data for all stocks.
- Pull information for one stock from one group.
- Show AAR, AAR-SD, CAAR and CAAR-SD for one group.
- Show the gnuplot graph with CAAR for all 3 groups.
- Exit program.     
 <br/> 

## Calculation Details:
1. From Bloomberg terminal, use a query to pull 2019 1st quarter earnings releases (if a company’s earning is far away from the 1st calendar quarter of 2019, select a quarterly earning close to 1st calendar quarter of 2019) for all S&P 500 stocks, sort and divide them into 3 groups:
    - Calculate earnings surprise for each stock:
Surprise % = (Actual EPS – EPS Estimate) ÷ EPS Estimate
    - Sort all the surprises in ascending order, and split all the stocks into 3 groups with relatively equivalent numbers of stocks:
        * Highest surprise group: Beat Estimate Group
        * Lowest surprise group: Miss Estimate Group
        * The rest stocks in between: Meet Estimate Group
2. Define day “zero” for a stock as the day the earning is announced
3. Implement Bootstrapping:
    - Sampling the stocks by randomly selecting 30 stocks from each group, total 90 stocks.
    - Use libcurl lib to retrieve 61 days of historical prices for S&P500 stocks and ETF ticker SPY around the date of earning release (You could enhance our class example for this purpose).
    - For each stock calculate the daily returns Rit for 30 days before the day “zero” and 30 days after: t = -30, -29,…-1, 0, 1,…, 29, 30: R<sub>it</sub> = (Price<sub>t</sub> - Price<sub>t-1</sub>)/Price<sub>t-1</sub>
Using adjusted daily closing price for your calculation
    - Calculate the corresponding daily return R<sub>mt</sub> for SPY for the same days.
    - Define abnormal returns as the difference AR<sub>it</sub>= R<sub>it</sub>–R<sub>mt</sub>.
    - Calculate average daily abnormal returns for each group of stocks for all 60 reference days: AAR<sub>t</sub>=1/N*ΣAR<sub>it</sub> from i=1 to N
    - Cumulate the returns on the first T days to CAAR: CAAR=ΣARR<sub>t</sub>  from t=−30 to 30
    - Repeat steps a to g 30 times and then Calculate
        * Average AAR and CAAR for 30 samplings for each group
        * Standard Deviation of AAR and CAAR for each group
4. Generate an Excel or gnuplot chart show the averaged CAAR of all three groups and discuss the impact the earning releases on their stock prices. Is there any conclusion you could draw from your project?


