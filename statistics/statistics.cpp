#include "statistics.h"

namespace Statistics{
    void Statistics::appdate(std::string s)
    {
        auto it = stat.find(s);
        if(it != stat.end()){
            stat[s] += 1;
        }else{
            stat.insert(std::pair<std::string, int>(s, 1));
        }
    }

    void Statistics::writeStatToFile(const std::string& filename, const std::string& recognizer_type) 
    {
        std::ofstream file(filename, std::ios::out);
        if(file.is_open()){
            std::cout << "File open..." << std::endl;
            file << "usage statistics for zone name strings by " << recognizer_type << "\n-------------------------------------\nzone_name – number_of_uses:\n";
            for(auto elem : stat){
                file << elem.first << " - " << elem.second << std::endl;
            }
        }else {
            std::cerr << "Error of open file: " << filename << std::endl;
        }
        file.close();
    }
}