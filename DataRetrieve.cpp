#include <string>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>  
#include <iomanip>
#include <locale>
#include <thread>
#include <curses.h>
#include "StockData.h"
#include "DataRetrieve.h"
#include "curl/curl.h"

using namespace std;


int write_data(void* ptr, int size, int nmemb, FILE* stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}


void* myrealloc(void* ptr, size_t size)
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}


int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

/** \calculate the start time or end time in seconds.
* \Time: report time from Bloomberg
* \start: 1 for start time (60 days before report date), 0 for end time(60 days after report date)
* \return: string for start time or end time in seconds
*/
string getTimeinSeconds(string Time, int start)
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y-%m-%d"))
	{
		if (start) {
			sprintf(time, "%lld", mktime(&t) - 60 * 60 * 60 * 24);
			return string(time);
		}
		else {
			sprintf(time, "%lld", mktime(&t) + 60 * 60 * 60 * 24);
			return string(time);
		}
	}
	else
	{
		cout << "Parse failed\n";
		return "";
	}
}

/** \read data from a csv file from Bloomberg and store the data in a map
* \mapDate: store data. key: stock index; value.sur: earning surprise; value.reportDate: report Date
*
* Sur.csv store the stock index, report date and earning surprise. This method reads these data from csv
* and use a map to store these data.
*/
void BloombergData(map<string, StockData>& mapData)
{
	StockData stock;

	ifstream inFile("Sur_new.csv", ios::in);
	if (!inFile)
	{
		cout << "There's something Wrong" << endl;
		exit(1);
	}
	int i = 0;
	string line;
	string field;
	string index, reportDate, sur;
	while (getline(inFile, line))
	{
		string field;
		istringstream sin(line);

		getline(sin, field, ',');
		index = string(field.c_str());

		getline(sin, field, ',');
		stock.reportDate = string(field.c_str());

		getline(sin, field, ',');
		stock.sur = atof(field.c_str());
		mapData.insert(pair<string, StockData>(index, stock));
		i++;
	}
	inFile.close();
}

/** \retrieve data from Yahoo finance and fill data in the map
* \mapDate: store data. key: stock index; value.sDate: trading dates; value.sValue: adjusted close price
* \return: whether the method is correctly executed.
*
* Go through the map and use the index to crawl trading data from Yahoo finance. Retrieved trading dates and
* adjusted close prices are filled in the map's value.
*/
int StockRetrieve(map<string, StockData>& mapData) 
{
	auto itr = mapData.begin();
    int stock_size = mapData.size();
    int stock_num = 1;
	struct MemoryStruct data;

	// file pointer to create file that store the data  
	FILE* fp;

	// name of files  
	const char outfilename[FILENAME_MAX] = "Output.txt";

	// declaration of an object CURL 
	CURL* handle;

	CURLcode result;

	// set up the program environment that libcurl needs 
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle 
	handle = curl_easy_init();

	// if everything's all right with the easy handle... 
	if (handle)
	{
		string sCookies, sCrumb;
		while (true)
		{
			data.memory = NULL;
			data.size = 0;
			
			if (sCookies.length() == 0 || sCrumb.length() == 0)
			{
				fp = fopen(outfilename, "w");
				curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
				curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
				curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

				curl_easy_setopt(handle, CURLOPT_ENCODING, "");
				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
				result = curl_easy_perform(handle);
				fclose(fp);

				if (result != CURLE_OK)
				{
					// if errors have occurred, tell us what is wrong with result
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
					return 1;
				}

				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

				// perform, then store the expected code in result
				result = curl_easy_perform(handle);

				if (result != CURLE_OK)
				{
					// if errors have occured, tell us what is wrong with result
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
					return 1;
				}

				char cKey[] = "CrumbStore\":{\"crumb\":\"";
				char* ptr1 = strstr(data.memory, cKey);
				char* ptr2 = ptr1 + strlen(cKey);
				char* ptr3 = strstr(ptr2, "\"}");
				if (ptr3 != NULL)
					*ptr3 = NULL;

				sCrumb = ptr2;

				fp = fopen("cookies.txt", "r");
				char cCookies[100];
				if (fp) {
					while (fscanf(fp, "%s", cCookies) != EOF);
					fclose(fp);
				}
				else
					cerr << "cookies.txt does not exists" << endl;

				sCookies = cCookies;
				free(data.memory);
				data.memory = NULL;
				data.size = 0;
			}

			if (itr == mapData.end())
				break;

			string symbol = itr->first;
			string startTime, endTime;

			if (symbol == "SPY")
			{
				startTime = getTimeinSeconds("2019-01-01", 1);
				endTime = getTimeinSeconds("2019-03-30", 0);
			}
			else
			{
				startTime = getTimeinSeconds((itr->second).reportDate, 1);
				endTime = getTimeinSeconds((itr->second).reportDate, 0);
			}

			string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
			string urlB = "?period1=";
			string urlC = "&period2=";
			string urlD = "&interval=1d&events=history&crumb=";
			string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
			const char* cURL = url.c_str();
			const char* cookies = sCookies.c_str();
			curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
			curl_easy_setopt(handle, CURLOPT_URL, cURL);
			//curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, FALSE);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				// if errors have occurred, tell us what is wrong with result
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			//save date and adjusted close price in the map
			stringstream sData;
			vector<string> stockDates;
			vector<float> stockValues;

			sData.str(data.memory);
			string sValue, sDate;
			double dValue;
			string line;
			getline(sData, line);
			while (getline(sData, line)) {
				sDate = line.substr(0, line.find_first_of(','));
				line.erase(line.find_last_of(','));
				sValue = line.substr(line.find_last_of(',') + 1);
				dValue = strtod(sValue.c_str(), NULL);
				stockDates.push_back(sDate);
				stockValues.push_back(dValue);
			}
			(itr->second).sDate = stockDates;
			(itr->second).sValue = stockValues;
			//cout << symbol << '\t' << stockDates.size() << endl;
			itr++;
		}
        free(data.memory);
        data.size = 0;
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	// cleanup since you've used curl_easy_init  
	curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init() 
	curl_global_cleanup();

	return 0;
}


/** \wrap StockRetrieve method and use multi-thread to speed up data retrieve
* \mapDate: store data.
*
* A tentative trial of using multi threads for data crawling. To avoid multi threads accessing the map at the same time,
* I copy the map and split it into 4 small map. It does create some overhead, but it greatly improve the speed of crawling.
* By using multi threads, the expected time for crawling data from Yahoo finance is about 20 seconds.
*/
typedef map<string, StockData> mapStruct;
void multiThread(map<string, StockData>& mapData)
{
 vector<mapStruct> mapVector(8);
 int count = 0;
 int batch_size = int(mapData.size() / 8);
 for (auto it = mapData.begin(); it != mapData.end(); it++)
 {
  if (count <= batch_size)
   mapVector[0].insert(*it);
  else if (count <= 2 * batch_size && count > batch_size)
   mapVector[1].insert(*it);
  else if (count <= 3 * batch_size && count > 2*batch_size)
   mapVector[2].insert(*it);
  else if (count <= 4 * batch_size && count > 3 * batch_size)
   mapVector[3].insert(*it);
  else if (count <= 5 * batch_size && count > 4 * batch_size)
   mapVector[4].insert(*it);
  else if (count <= 6 * batch_size && count > 5 * batch_size)
   mapVector[5].insert(*it);
  else if (count <= 7 * batch_size && count > 6 * batch_size)
   mapVector[6].insert(*it);
  else
   mapVector[7].insert(*it);
  count++;
 }
 
 thread t0{ StockRetrieve,  ref(mapVector[0])};
 thread t1{ StockRetrieve,  ref(mapVector[1])};
 thread t2{ StockRetrieve,  ref(mapVector[2])};
 thread t3{ StockRetrieve,  ref(mapVector[3])};
 thread t4{ StockRetrieve,  ref(mapVector[4]) };
 thread t5{ StockRetrieve,  ref(mapVector[5]) };
 thread t6{ StockRetrieve,  ref(mapVector[6]) };
 thread t7{ StockRetrieve,  ref(mapVector[7]) };

 t0.join();
 t1.join();
 t2.join();
 t3.join();
 t4.join();
 t5.join();
 t6.join();
 t7.join();
 
 for(int i = 1; i<8;i++)
  mapVector[0].insert(mapVector[i].begin(), mapVector[i].end());

 mapData = mapVector[0];
}
