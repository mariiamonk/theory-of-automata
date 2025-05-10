#ifndef LAB2_PROG_H
#define LAB2_PROG_H
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../Regex/Regex.h"

class Prog {
private:
    // Encapsulated helper methods
    void displayMainMenu() const {
        std::cout << "\n=== Regular Expression Operations ===" << std::endl;
        std::cout << "1. Full match verification" << std::endl;
        std::cout << "2. Substring search" << std::endl;
        std::cout << "3. Display automaton" << std::endl;
        std::cout << "4. Reconstruct regex pattern" << std::endl;
        std::cout << "5. Invert regex pattern" << std::endl;
        std::cout << "6. Language difference" << std::endl;
        std::cout << "7. Exit" << std::endl;
        std::cout << "Select operation: ";
    }

    std::string getInput(const std::string& prompt) const {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }

    // Operation implementations
    void executeMatch() const {
        std::string pattern = getInput("Enter regex pattern: ");
        std::string text = getInput("Enter test string: ");

        bool result = Regex::Regex::match(pattern, text);
        std::cout << "The string " << (result ? "matches" : "does not match")
                  << " the pattern completely" << std::endl;
    }

    void executeSearch() const {
        std::string pattern = getInput("Enter regex pattern: ");
        std::string text = getInput("Enter text to search: ");

        Regex::Regex re(pattern);
        auto result = re.search(text);

        if (result) {
            std::cout << "Match found: " << result.match << std::endl;
            if (!result.groups.empty()) {
                std::cout << "Capture groups:" << std::endl;
                for (const auto& [name, value] : result.groups) {
                    std::cout << "  " << (name.empty() ? "Main" : name)
                              << ": " << value << std::endl;
                }
            }
        } else {
            std::cout << "No matches found!" << std::endl;
        }
    }

    void displayAutomaton() const {
        std::string pattern = getInput("Enter regex pattern: ");
        Regex::Regex re(pattern);
        re.printAutomata();
    }

    void reconstructPattern() const {
        std::string pattern = getInput("Enter regex pattern: ");
        Regex::Regex re(pattern);
        std::cout << "Reconstructed pattern: " << re.toRegex(5) << std::endl;
    }

    void invertPattern() const {
        std::string pattern = getInput("Enter regex pattern: ");
        Regex::Regex re(pattern);
        Regex::Regex inverted = re.inverse();
        std::cout << "Inverted pattern: " << inverted.toRegex(3) << std::endl;
    }

    void calculateDifference() const {
        std::string pattern1 = getInput("Enter first regex pattern: ");
        std::string pattern2 = getInput("Enter second regex pattern: ");

        Regex::Regex re1(pattern1);
        Regex::Regex re2(pattern2);
        Regex::Regex diff = re1.difference(re2);

        std::cout << "Language difference result: " << diff.toRegex(3) << std::endl;
    }

public:
    // Main application loop
    void run() {
        int choice = 0;

        while (true) {
            displayMainMenu();
            std::cin >> choice;
            std::cin.ignore(); // Clear input buffer

            try {
                switch (choice) {
                    case 1: executeMatch(); break;
                    case 2: executeSearch(); break;
                    case 3: displayAutomaton(); break;
                    case 4: reconstructPattern(); break;
                    case 5: invertPattern(); break;
                    case 6: calculateDifference(); break;
                    case 7: return;
                    default: std::cout << "Invalid selection!" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
};


#endif //LAB2_PROG_H
