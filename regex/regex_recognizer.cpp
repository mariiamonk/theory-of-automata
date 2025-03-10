#include "regex_recognizer.h"

namespace Regex{

    bool RegexRecognizer::validate(const std::string& filename){
        std::ifstream file(filename, std::ios::in);
        if(!file){
            std::cerr << "Error of open file: " << filename << std::endl;
            return false;
        }
    
        std::string line;

        std::string protocol = "^(http://|www.)";
        std::string name     = "([a-zA-Z0-9]{1,20})";
        std::string zone     = "([a-zA-Z]{1,5})$";
    
        std::regex pattern(protocol + name + '.' + name + '.' + zone, std::regex_constants::optimize);  
        std::smatch match;
    
        while (std::getline(file, line))
        {
            if (std::regex_match(line, match, pattern)) {
                stat.update(match[4]);
            }
        }
    
        stat.writeStatToFile("result.txt", "REGEX");
    
        file.close();
        return true;
    }

    void RegexRecognizer::timer(){
        std::ofstream timerFile("regex/test_data/timer_regex.txt");
        if (!timerFile) {
            throw std::runtime_error("Could not open timer file for writing");
        }

        std::string line;

        std::regex pattern(R"(^(http://|www\.)([a-zA-Z0-9]{1,20})\.([a-zA-Z0-9]{1,20})\.([a-zA-Z]{1,5})$)", std::regex_constants::optimize);

        std::smatch match;

        double timer_taken;
        std::vector <std::string> type_string = {"correct ", "uncorrect ", "random "};

        for(int j = 0; j < 3; j++){
            timerFile << "Time taken by REGEX for " << type_string[j] << "(sec) :" << std::endl;
            for (int i = 50; i <= 140000; i*=2) {
                std::string filename = "regex/test_data/testsData_" + std::to_string(i) + "_nums.txt";

                Generator::StringGenerator g;
                g.generateToFile(filename, i, j);

                std::ifstream inputFile(filename, std::ios::in);
                if (!inputFile.is_open()) {
                    std::cerr << "Error opening file for reading: " << filename << std::endl;
                    continue;
                }
        
                timer_taken = 0;
                while (std::getline(inputFile, line))
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    std::regex_match(line, match, pattern);
                    auto end = std::chrono::high_resolution_clock::now();
                    timer_taken += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
                }
                timer_taken *= 1e-9;
                timerFile << i << " : " << std::fixed << timer_taken << std::endl;
                inputFile.close();
            }
            timerFile << std::endl;
        }
        timerFile.close();
    }
}