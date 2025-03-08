#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <random>
#include <ctime>
#pragma once

namespace Generator{
    class StringGenerator{
        std::mt19937 gen;
        std::uniform_int_distribution<> charDist;
    public:
        StringGenerator() : gen(std::time(nullptr)), charDist(0, 25){}

        std::string generateRandomString(size_t length, bool UpLetters);
        std::string generateRandomNumberString(size_t length);

        std::string generateCorrectString();
        std::string generateIncorrectString();

        void generateToFile(const std::string& filename, size_t count, int correctString = 0);
    };
}