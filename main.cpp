#include <iostream>
#include "regex/regex_recognizer.h"
#include "statistics/statistics.h"
#include "fstream"

int main(){
    Hello::helloprint();
    Statistics::Statistics stat;
    stat.appdate("abc");
    stat.appdate("dfg");
    stat.appdate("acd");
    stat.appdate("abc");
    for(auto elem : stat.getStatistics()){
        std::cout << elem.first << "-" << elem.second << std::endl;
    }

    stat.writeStatToFile("result.txt", "smc");
}
