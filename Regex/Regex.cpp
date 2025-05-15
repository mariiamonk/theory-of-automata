#include "Regex.h"
#include <algorithm>
#include <iostream>

namespace Regex {

    Regex::Regex(const std::string& pattern) : pattern(pattern) {
        compile();
    }

    void Regex::compile() {
        ast = std::make_unique<AbstractTree::AST>(pattern);
        nfa = std::make_unique<NFA>(*ast);
        dfa = std::make_unique<DFA>(*nfa);
        minimizedDfa = std::make_unique<MinimizedDFA>(*dfa);
        compiled = true;
    }

    MatchResult Regex::search(const std::string& text) const {
        const_cast<Regex*>(this)->compile();
        MatchResult result;
        result.success = false;

        for (size_t i = 0; i < text.size(); ++i) {
            auto currentState = minimizedDfa->getStartState();
            size_t matchStart = i;
            size_t matchEnd = i;

            for (size_t j = i; j < text.size(); ++j) {
                auto it = currentState->transitions.find(text[j]);
                if (it == currentState->transitions.end()) break;

                currentState = it->second;
                matchEnd = j + 1;

                if (currentState->isFinal) {
                    if (!result.success || (matchEnd - matchStart) > result.match.size()) {
                        result.success = true;
                        result.match = text.substr(matchStart, matchEnd - matchStart);

                        result.groups.clear();
                        for (const auto& [name, data] : ast->getEnumData().groupsData) {

                            std::string groupMatch;
                            for (const auto& [name, data] : ast->getEnumData().groupsData) {
                                size_t groupStartPos = *data.beginWith.begin();
                                size_t groupEndPos = *data.endWith.rbegin();

                                if (groupStartPos <= groupEndPos && groupEndPos <= text.length()) {
                                    result.groups[name] = text.substr(groupStartPos - 1, groupEndPos - groupStartPos + 1);
                                }
                            }
                        }
                    }
                }
            }
        }

        return result;
    }

    bool Regex::test(const std::string& text) const {
        if (!compiled) throw std::runtime_error("Regex not compiled");
        return minimizedDfa->simulate(text);
    }

    bool Regex::match(const std::string& text) {
        return test(text);
    }

    std::string Regex::toRegex() const {
        if (!compiled) throw std::runtime_error("Regex not compiled");
        return minimizedDfa->toRegex();
    }

    void Regex::printAutomata() const {
        const_cast<Regex*>(this)->compile();

        std::cout << "=== AST ===" << std::endl;
        ast->print();

        std::cout << "=== NFA ===" << std::endl;
        nfa->print();
        nfa->visualizeNFA("nfa.png");

        std::cout << "\n=== DFA ===" << std::endl;
        dfa->print();
        dfa->visualizeDFA("dfa.png");

        std::cout << "\n=== Minimized DFA ===" << std::endl;
        minimizedDfa->print();
        minimizedDfa->visualize("minimized.png");
    }

    Regex Regex::inverse() const {
        if (!compiled) throw std::runtime_error("Regex not compiled");
        MinimizedDFA inverted = minimizedDfa->reverseLanguage();
        return Regex(std::move(inverted));
    }

    Regex Regex::difference(const Regex& other) const {
        if (!compiled || !other.compiled) throw std::runtime_error("Regex not compiled");
        MinimizedDFA diff = minimizedDfa->difference(*other.minimizedDfa);
        return Regex(std::move(diff));
    }
}