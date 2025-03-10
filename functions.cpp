#include "functions.h"

namespace prog
{
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

    void runNormalMode(const std::string& recognizerType, const std::string& filePath, int numStrings) {
        Generator::StringGenerator generator;

        generator.generateToFile(filePath, numStrings);


        std::unique_ptr<Recognizer> recognizer = createRecognizer(recognizerType);
        bool success = recognizer->validate(filePath);

        if (success) {
            std::cout << "Validation succeeded using " << recognizerType << " recognizer.\n";
        } else {
            std::cerr << "Validation failed.\n";
        }
    }
}