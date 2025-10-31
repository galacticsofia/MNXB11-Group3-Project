#include <iostream>
#include <fstream>
#include <string>
#include <vector>
// path to read the file and path to output cleaned dataset
const std::string IN_PATH = "../../datasets/SMHI_pthbv_p_t_1961_2025_daily_4326.csv";
const std::string OUT_PATH = "../data_clean/Rain_temperature_cleaned.csv";

std::string trim(const std::string& s){
    size_t a = s.find_first_not_of(" \t\n"); // find index of first character other than space and tabs
    if(a == std::string::npos) return ""; // if s has no characters and a is empty we return empty string
    size_t b = s.find_last_not_of(" \t\n"); // find index of last character other than space and tabs
    return s.substr(a, b - a + 1);
}

std::vector<std::string> split_semicolon(const std::string& s){
    /*
    The purpose of this function is to get a string like "1961-01-26;0.0;-7.2;0.0;-10.3" and transform it  into the form ["1961-01-26","0.0","-7.2","0.0","-10.3"] 
    
    We loop through each character and when we see ';', it stores the current text as one whole element
    and then starts collecting the next characters untill ";" comes up again.
    */
    std::vector<std::string> out;
    out.reserve(8); // preallocates size to 8 for better performance
    std::string current;
    for(char c : s){
        if(c == ';'){ 
            out.push_back(current); current.clear(); 
        }
        else current.push_back(c);
    }
    out.push_back(current);
    return out;
}

bool looks_like_date(const std::string& s){
    // checking if the argument is of the form YYYY-MM-DD
    if(s.size()!=10) return false;
    for(int i=0;i<10;i++){
        if(i==4 || i==7){
            if(s[i] != '-') return false;  
        }
        else if(!isdigit(s[i])) return false;
    }
    return true;
}

int main(){
    std::ifstream fin(IN_PATH); 
    std::ofstream fout(OUT_PATH);
    std::string line;
    int line_no = 0;
    int kept = 0, skipped = 0;

    // header to be added to the cleaned csv file
    fout << "date,rain_Lund_mm,temp_Lund_C,rain_Uppsala_mm,temp_Uppsala_C\n";

    // we skip the first two lines of the csv files as they are not relevant
    for(int i=0;i<2 && std::getline(fin, line); ++i){
        line_no++;
        // just skip
    }

    // Read data lines
    while(std::getline(fin, line)){
        line_no++;
        // retrive line and trim it 
        std::string t = trim(line);
        if(t.empty()){ skipped++; continue; } // if line empty make sure user knows a line is skipped

        //split_semicolon splits via semicolon
        //auto  tells the compiler to figure out the necessary type that needs to be assinged
        auto cols = split_semicolon(t);
        if(cols.size() < 5){ skipped++; continue; }

        std::string date = trim(cols[0]); // take out the date from cols and trim it
        if(!looks_like_date(date)){ skipped++; continue; } // if line empty make sure user knows a line is skipped

        
        std::string rA = cols[1]; // rain of city A : Lund
        std::string tA = cols[2]; // temperature of city A
        std::string rB = cols[3]; // rain of city B : Uppsala
        std::string tB = cols[4]; // temperature of city B

        
        fout << date << ",";
        fout << rA   << ",";
        fout << tA   << ",";
        fout << rB   << ",";
        fout << tB   << "\n";
        kept++;
    }
    // we print this as a precaution to make sure no line is skipped
    std::cout << "Cleaning done , cleaned CSV: " << OUT_PATH << " | rows kept: " << kept << ", rows skipped: " << skipped << "\n";
    return 0;
}
