#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <ctime>

class StringGenerator{

    std::mt19937 gen;
    std::uniform_int_distribution<> charDist;
    std::uniform_int_distribution<> lengthDist;

public:
    StringGenerator() : gen(std::time(nullptr)), charDist(0, 25), lengthDist(1, 20) {}

    std::string generateRandomString(size_t length, bool UpLetters);
    std::string generateRandomNumberString(size_t length);

    std::string generateCorrectString();
    std::string generateIncorrectString();

    void generateToFile(const std::string& filename, size_t count);
};