#include "regex_recognizer.h"

namespace Regex{

    bool RegexRecognizer::validate(const std::string& filename){
        std::ifstream file(filename, std::ios::in);
        if(!file.is_open()){
            std::cerr << "Error of open file: " << filename << std::endl;
            return false;
        }

        std::string line;

        std::regex pattern(R"(^(http:\/\/|www\.)([a-zA-Z0-9]{1,20})\.([a-zA-Z0-9]{1,20})\.([a-zA-Z]{1,5})$)");
        std::smatch match;

        while (std::getline(file, line))
        {
            if(std::regex_match(line, match, pattern)){
                stat.appdate(match[4]);
            }
        }
        

        if(!stat.writeStatToFile("result.txt", "REGEX"))return false;

        file.close();

        return true;
    }

    void RegexRecognizer::timer(){
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();
        double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        time_taken *= 1e-9;
        std::cout << "Time taken by REGEX is : " << std::fixed << time_taken << " sec" << std::endl;
    }
}