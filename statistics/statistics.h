#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>
#include <string.h>

#pragma once

namespace Statistics{
    class Statistics{
            std::map <std::string, int> stat;
        public:
            Statistics() = default;

            inline void clear(){
                stat.erase(stat.begin(), stat.end());
            }
            
            void update(std::string s);

            inline auto getStatistics(){
                return stat;
            }

            void writeStatToFile(const std::string& filename, const std::string& recognizer_type);
        };
}
    