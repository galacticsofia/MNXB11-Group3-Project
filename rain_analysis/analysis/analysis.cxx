// Build: g++  analysis.cxx -o analysis

// ./analysis ../data_clean/Rain_temperature_cleaned.csv 1961 A ../results/monthly_Lund_1961.csv
// ./analysis ../data_clean/Rain_temperature_cleaned.csv 2024 B ../results/monthly_Uppsala_2024.csv

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

std::vector<std::string> split_csv(const std::string& s){
    /*
    // Splits a comma-separated line like "1961-04-05,9.0,4.5,2.5,-0.0" into ["1961-04-05","9.0","4.5","2.5","-0.0"].

    */
    std::vector<std::string> v;
    std::string current;
    for(char c: s){
        if(c==','){ 
            v.push_back(current);
            current.clear();
        } 
        else current.push_back(c); 
    }
    v.push_back(current); 
    return v;
}

/*
How  char** argv works 

char** argv is a pointer to pointer to a char

When we define:
    int main(int argc, char** argv)

it means our program can receive **command-line arguments** — the words typed after
the program name in the terminal.

We know  that 
    char   → a single character ('A')
    char*  → a pointer to a char → represents a string
    char** → a pointer to a pointer to a char → represents an array of strings

char c = 'A'; // c stores char 'A'
char* p = &c; // p contains address of c

char word[] = "Hello"; 
//In the memory it is stored as:  H  e  l  l  o  \0
\0 represents null terminator which indicates the end of the string
char* s = word; // this makes s point to 'H' , thus char* is a pointer to a char representing a string

Similarly,
    char* fruits[] = {"apple", "banana", "pear"};
can be visualised as 
    fruits
        ├──> "apple"  ──> ['a','p','p','l','e','\0']
        ├──> "banana" ──> ['b','a','n','a','n','a','\0']
        └──> "pear"   ──> ['p','e','a','r','\0']

So fruits is an array of char*, meaning each element is a pointer to a different string.

Lastly,
    char** is a pointer to a pointer to a char which is a pointer to an array of strings


*/

int main(int argc, char** argv){
    /*
    the main method takes two arguments : int argc and char** argv

    in the terminal when we write
    ./analysis data_clean/Rain_temperature_cleaned.csv 2024 B results/monthly_Uppsala_2024.csv

    it is passed as 5 arguments as in int argc = 5
    char** argv takes in the arguments and stores it in itself like : 
        argv[0] = "./analysis"
        argv[1] = "data_clean/Rain_temp_cleaned.csv"
        argv[2] = "1961"
        argv[3] = "A"
        argv[4] = "results/monthly_A_1961.csv"

    
    */
    if(argc < 5){
        std::cerr << "Usage: " << argv[0]
                  << " <input_csv> <year> <station(A|B)> <output_csv>\n";
        return 1;
        // checking if correct number of arguments are given or will raise error message
    }
    const std::string in_csv   = argv[1]; // example argv[1] = "data_clean/Rain_temp_cleaned.csv"
    const int         year_sel = std::stoi(argv[2]); // std::stoi string to integer
    const char        station  = argv[3][0];
    const std::string out_csv  = argv[4];

    // ternary conditional operator (condition) ? value_if_true : value_if_false
    const int idxRain = (station=='A' ? 1 : 3); // If station=='A', use columns 1 & 2; otherwise 3 & 4.
    const int idxTemp = (station=='A' ? 2 : 4); 


    std::ifstream f(in_csv);
    if(!f.is_open()){
        std::cerr << "ERROR: cannot open " << in_csv << "\n";
        return 2;
        // check to see if we can use our clean dataset csv
    }

    // monthly data point
    double month_rain_sum[13] = {0};  
    int    month_rainy_days[13] = {0};
    double month_tmax[13]; for(int m=1;m<=12;++m) month_tmax[m] = -1e300;
    double month_tmin[13]; for(int m=1;m<=12;++m) month_tmin[m] =  1e300;
    bool   month_seen[13] = {false};

    std::string line;
    if(!std::getline(f, line)){ std::cerr << "ERROR: empty file\n"; return 3; } // we check if the header is there or not in the cleaned dataset csv

    while(std::getline(f, line)){
        if(line.empty()) continue;
        auto cols = split_csv(line); // take line from the csv, split it and save it to cols, auto determines type on it own
        if((int)cols.size() < 5) continue;

        /*
        stoi() converts string to int  
            example :  "123" → 123
        substr() — extract part of a string only 
            example :  if date = "2025-10-30" then  date.substr(0, 4) gives "2025"
        */
        
        std::string date = cols[0]; // YYYY-MM-DD
        if (date.size() < 10) continue;

        int y = std::stoi(date.substr(0, 4));
        if (y != year_sel) continue;

        int m = std::stoi(date.substr(5, 2));
        if (m < 1 || m > 12) continue;

        // std::stod converts a string to double
        double rain = cols[idxRain].empty() ? 0.0 : std::stod(cols[idxRain]);
        double temp = cols[idxTemp].empty() ? NAN  : std::stod(cols[idxTemp]);


        // rainfall
        if(rain > 0.0) month_rainy_days[m] += 1;
        month_rain_sum[m] += std::max(0.0, rain);

        // temperature extremes
        if(!std::isnan(temp)){
            month_tmax[m] = std::max(month_tmax[m], temp);
            month_tmin[m] = std::min(month_tmin[m], temp);
            month_seen[m] = true;
        }
    }
    f.close();

    // Replacing months with no data with zeros for better plotting
    for(int m=1;m<=12;++m){
        if(!month_seen[m]){ month_tmax[m]=0.0; month_tmin[m]=0.0; }
    }

    std::ofstream out(out_csv);
    if(!out.is_open()){
        std::cerr << "ERROR: cannot open " << out_csv << " for writing\n";
        return 4;
    }
    out << "month,total_rain_mm,monthly_tmax_C,monthly_tmin_C,rainy_days\n";
    for(int m=1;m<=12;++m){
        out << m << ","
            << month_rain_sum[m] << ","
            << month_tmax[m]     << ","
            << month_tmin[m]     << ","
            << month_rainy_days[m] << "\n";
    }
    out.close();

    std::cout << "Wrote " << out_csv
              << " for station " << station
              << " and year " << year_sel << "\n";
    return 0;
}
