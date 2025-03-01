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

        std::string generateRandomString(size_t length, bool UpLetters) {
        std::string result;
        for (size_t i = 0; i < length; ++i) {
            char c = 'a' + charDist(gen) % 26;
            if (UpLetters && charDist(gen) % 2) {
                c = toupper(c);
            }
            result += c;
        }
        return result;
    }

    std::string generateRandomNumberString(size_t length) {
        std::string result;
        for (size_t i = 0; i < length; ++i) {
            result += '0' + charDist(gen) % 10;
        }
        return result;
    }

    std::string generateCorrectString() {
        std::string url = "http://";
        if (charDist(gen) % 2) {
            url += "www.";
        }
        url += generateRandomString(lengthDist(gen), true) + ".";
        url += generateRandomString(lengthDist(gen), true) + ".";
        url += generateRandomString(1 + charDist(gen) % 5, false);
        return url;
    }

    std::string generateIncorrectString() {
        std::string url;
        if (charDist(gen) % 2) {
            url += "http://";
        }
        if (charDist(gen) % 2) {
            url += "www.";
        }
        url += generateRandomString(lengthDist(gen), true) + ".";
        url += generateRandomString(lengthDist(gen), true) + ".";
        url += generateRandomNumberString(1 + charDist(gen) % 5);
        return url;
    }

    void generateToFile(const std::string& filename, size_t count) {
        std::ofstream outFile(filename);
        if (!outFile) {
            throw std::runtime_error("Could not open file for writing.");
        }

        for (size_t i = 0; i < count; ++i) {
            std::string line = charDist(gen) % 2 ? generateCorrectString() : generateIncorrectString();
            outFile << line << std::endl;
        }
    }
};