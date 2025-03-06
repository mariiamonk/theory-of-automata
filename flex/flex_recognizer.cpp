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
            stringstream_ << line << "\n"; 
            ftp.yyrestart(&stringstream_); 

            if (ftp.yylex()) { 
                recognizedText = ftp.YYText();
                recognizedText.pop_back(); 
                stat.appdate(recognizedText); 
            }

            stringstream_.clear();
        }

        if(!stat.writeStatToFile("result.txt", "FLEX"))return false;

        file.close();

        return true;
    }

    void FlexRecognizer::timer(){
        std::cout << "timer\n";
    }
}