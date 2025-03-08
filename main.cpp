#include "regex/regex_recognizer.h"
#include "flex/flex_recognizer.h"
#include "statistics/statistics.h"
#include "generator/generator.h"
#include <chrono>
#include <iostream>
#include <memory>

std::unique_ptr<Recognizer> createRecognizer(const std::string& type) {
    if (type == "regex") {
        return std::make_unique<Regex::RegexRecognizer>();
    } else if (type == "flex") {
        return std::make_unique<Flex::FlexRecognizer>();
    } else {
        throw std::invalid_argument("Unknown recognizer type: " + type);
    }
}

void runTimerMode(const std::string& recognizerType, const std::string& filePath) {
    std::unique_ptr<Recognizer> recognizer = createRecognizer(recognizerType);
    recognizer->timer();
}

void runNormalMode(const std::string& recognizerType, const std::string& filePath, int numStrings, std::string no_generate = "") {
    Generator::StringGenerator generator;

    if (no_generate != "no_generate"){
        generator.generateToFile(filePath, numStrings);
    }

    std::unique_ptr<Recognizer> recognizer = createRecognizer(recognizerType);
    bool success = recognizer->validate(filePath);

    if (success) {
        std::cout << "Validation succeeded using " << recognizerType << " recognizer.\n";
    } else {
        std::cerr << "Validation failed.\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <recognizer_type> <file_path> <mode(timer/normal)> <num_strings(optional)> <no_generate(optional)>\n";
        std::cerr << "Recognizer types: regex, flex\n";
        return 1;
    }

    std::string recognizerType = argv[1];
    std::string filePath = argv[2];
    std::string mode = argv[3];

    try {
        if (mode == "timer") {
            runTimerMode(recognizerType, filePath);
        } else if (mode == "normal") {
            int numStrings = 10; 
            if (argc > 4) {
                numStrings = std::stoi(argv[4]);
            }
            runNormalMode(recognizerType, filePath, numStrings, argv[5]);
        } else {
            std::cerr << "Unknown mode: " << mode << ". Available modes: timer, normal.\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}