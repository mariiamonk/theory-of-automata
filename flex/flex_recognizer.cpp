#include "flex_recognizer.h"

namespace Flex{

    bool FlexRecognizer::validate(const std::string& filename){
        std::ifstream file(filename, std::ios::in);
        if(!file.is_open()){
            std::cerr << "Error of open file: " << filename << std::endl;
            return false;
        }

        std::string line;
        yyFlexLexer ftp;
        
        std::stringstream stringstream_;
        std::string recognizedText;

        while(std::getline(file, line))
        {
            stringstream_.str(line + "\n");  
            stringstream_.clear();           
            ftp.yyrestart(&stringstream_);   
            bool token = ftp.yylex();
            if (token) {  
                recognizedText = ftp.YYText();
                if (!recognizedText.empty() && recognizedText.back() == '\n') {
                    recognizedText.pop_back();
                }
                stat.update(recognizedText);
            }
        }

        stat.writeStatToFile("result.txt", "FLEX");

        file.close();

        return true;
    }

    void FlexRecognizer::timer(){
        std::ofstream timerFile("flex/test_data/timer_flex.txt");
        if (!timerFile) {
            throw std::runtime_error("Could not open timer file for writing");
        }

        yyFlexLexer ftp;
        std::string line;

        std::stringstream stringstream_;
        ftp.yyrestart(stringstream_);

        double timer_taken{};
        std::vector <std::string> type_string = {"correct ", "uncorrect ", "random "};

        for(int j = 0; j < 3; j++){
            timerFile << "Time taken by REGEX for " << type_string[j] << "(sec) :" << std::endl;
            for (int i = 50; i <= 140000; i*=2) {
                std::string filename = "flex/test_data/testsData_" + std::to_string(i) + "_nums.txt";

                Generator::StringGenerator g;
                g.generateToFile(filename, i, j);

                timer_taken = 0;

                std::ifstream inputFile(filename, std::ios::in);
                if (!inputFile.is_open()) {
                    std::cerr << "Error opening file for reading: " << filename << std::endl;
                    continue;
                }
        
                while (std::getline(inputFile, line))
                {
                    stringstream_ << line << "\n";
                    auto start = std::chrono::high_resolution_clock::now();
                    ftp.yylex();
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