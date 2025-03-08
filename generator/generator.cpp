#include "generator.h"

namespace Generator{
    std::string StringGenerator::generateRandomString(size_t length, bool UpLetters = true) {
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

    std::string StringGenerator::generateRandomNumberString(size_t length) {
        std::string result;
        for (size_t i = 0; i < length; ++i) {
            result += '0' + charDist(gen) % 10;
        }
        return result;
    }

    std::string StringGenerator::generateCorrectString() {
        std::uniform_int_distribution<> lengthDist(1, 20);
        std::string url;
        if (charDist(gen) % 2) {
            url += "www.";
        }else{
            url = "http://";
        }
        url += generateRandomString(lengthDist(gen)) + ".";
        url += generateRandomString(lengthDist(gen)) + ".";
        url += generateRandomString(1 + charDist(gen) % 5);
        return url;
    }

    std::string StringGenerator::generateIncorrectString() {
        std::uniform_int_distribution<> lengthDist(1, 100);
        std::string url;
        if (charDist(gen) % 2) {
            url += "http://";
        }
        if (charDist(gen) % 2) {
            url += "www.";
        }
        url += generateRandomString(lengthDist(gen)) + ".";
        url += generateRandomString(lengthDist(gen)) + ".";
        url += generateRandomNumberString(1 + charDist(gen) % 5);
        return url;
    }

    void StringGenerator::generateToFile(const std::string& filename, size_t count, int correctString) {
        std::uniform_int_distribution<> lengthDist;

        std::ofstream outFile(filename);
        if (!outFile) {
            throw std::runtime_error("Could not open file for writing");
        }

        switch (correctString)
        {
        case 0:
            for (size_t i = 0; i < count; ++i) {
                std::string line = charDist(gen) % 2 ? generateCorrectString() : generateIncorrectString();
                outFile << line << std::endl;
            }
            break;

        case 1:
            for (size_t i = 0; i < count; ++i) {
                std::string line = generateCorrectString();
                outFile << line << std::endl;
            }
            break;

        case 2:
            for (size_t i = 0; i < count; ++i) {
                std::string line = generateIncorrectString();
                outFile << line << std::endl;
            }
            break;

        default:
            break;
        }
    }
}