#include "regex_recognizer.h"
///^(http:\/\/|www.)(\w+\d*){1,20}.(\w+\d*){1,20}.([a-z]){1,5}$

namespace Regex{

    bool RegexRecognizer::validate(const std::string& filename){
        std::regex pattern(R"(^(http:\/\/)?(www\.)?([a-zA-Z0-9]{1,20})\.([a-zA-Z0-9]{1,20})\.([a-z]{1,5})$)");
        std::smatch match;

        std::ifstream file(filename, std::ios::in);
        if(!file.is_open()){
            std::cerr << "Error of open file: " << filename << std::endl;
            return false;
        }

        std::cout << "File open for reading" << std::endl;
        std::string line;

        while (std::getline(file, line))
        {
            if(std::regex_match(line, match, pattern)){
                stat.appdate(match[5]);
            }
        }

        if(!stat.writeStatToFile("result.txt", "REGEX"))return false;
        std::cout << "Writing result to file" << std::endl;

        return true;
    }
}