#include "Regex.h"
#include <algorithm>
#include <iostream>

namespace Regex {

    Regex::Regex(const std::string& pattern) : pattern(pattern) {
        ast = std::make_unique<AbstractTree::AST>(pattern);
    }

    void Regex::compile() {
        if (compiled) return;
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


    MatchResult Regex::search(const std::string& pattern, const std::string& text) {
        return Regex(pattern).search(text);
    }

    bool Regex::test(const std::string& text) const {
        const_cast<Regex*>(this)->compile();
        return minimizedDfa->simulate(text);
    }

    std::string Regex::toRegex(int k = -1) const {
        const_cast<Regex*>(this)->compile();
        return minimizedDfa->toRegex(k);
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
        const_cast<Regex*>(this)->compile();
        Regex result(this->pattern);
        result.minimizedDfa = std::make_unique<MinimizedDFA>(this->minimizedDfa->complement());
        result.compiled = true;
        return result;
    }

    Regex Regex::difference(const Regex& other) const {
        const_cast<Regex*>(this)->compile();
        const_cast<Regex&>(other).compile();
        Regex result(this->pattern + " - " + other.pattern);
        result.minimizedDfa = std::make_unique<MinimizedDFA>(
                this->minimizedDfa->difference(*other.minimizedDfa));
        result.compiled = true;
        return result;
    }

    bool Regex::match(const std::string& text) const {

    }
}