#include "flex_recognizer.h"

namespace Flex{

    bool FlexRecognizer::validate(const std::string& filename){
        std::ifstream file(filename, std::ios::in);
        if(!file.is_open()){
            std::cerr << "Error of open file: " << filename << std::endl;
            return false;
        }

        std::cout << "File open for reading" << std::endl;
        std::string line;

        std::stringstream stringstream_;

        yyFlexLexer ftp;
        ftp.yyrestart(stringstream_);

        auto start = std::chrono::high_resolution_clock::now();

        while(std::getline(file, line))
        {
            stringstream_ << line << "\n";
            if(ftp.yylex()) {
                stat.appdate(ftp.YYText());
            }
        }
        auto end = std::chrono::high_resolution_clock::now();

        double time_taken = 
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
 
        time_taken *= 1e-9;
 
        std::cout << "Time taken by FLEX is : " << std::fixed << time_taken << " sec" << std::endl;

        if(!stat.writeStatToFile("result.txt", "FLEX"))return false;
        std::cout << "Writing result to file" << std::endl;

        file.close();

        return true;
    }
}