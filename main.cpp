#include "functions.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <recognizer_type> <file_path> <mode(timer/normal)> <num_strings(optional)>\n";
        std::cerr << "Recognizer types: regex, flex\n";
        return 1;
    }
    std::string recognizerType = argv[1];
    std::string filePath = argv[2];
    std::string mode = argv[3];

    try {
        if (mode == "timer") {
            prog::runTimerMode(recognizerType, filePath);
        } else if (mode == "normal") {
            int numStrings = 10; 
            if (argc > 4) {
                numStrings = std::stoi(argv[4]);
            }
            prog::runNormalMode(recognizerType, filePath, numStrings);
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