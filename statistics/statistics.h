#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>
#include <string.h>

#pragma once

namespace Statistics{
    class Statistics{
            std::map <std::string, long int> stat;
        public:
            Statistics() = default;

            inline void clear(){
                stat.erase(stat.begin(), stat.end());
            }
            
            void appdate(std::string s);

            inline auto getStatistics(){
                return stat;
            }

            bool writeStatToFile(const std::string& filename, const std::string& recognizer_type);
        };
}
    