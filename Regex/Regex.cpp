#include "Regex.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace Regex {

    Regex::Regex(std::string pattern) : pattern(std::move(pattern)) {
        compile();
    }

    void Regex::compile() {
        ast = std::make_unique<AbstractTree::AST>(pattern);
        nfa = std::make_unique<NFA>(*ast);
        dfa = std::make_unique<DFA>(*nfa);
        minimizedDfa = std::make_unique<MinimizedDFA>(*dfa);
        compiled = true;
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

    void Regex::printAutomata(const std::string& name) const {
        //const_cast<Regex*>(this)->compile();

        std::cout << "\n=== Minimized DFA ===" << std::endl;
        minimizedDfa->print();
        minimizedDfa->visualize(name + "minimized.png");
    }

    Regex Regex::inverse() const {
        if (!compiled) throw std::runtime_error("Regex not compiled");
        return Regex(minimizedDfa->reverseLanguage());
    }

    Regex Regex::difference(const Regex& other) const {
        if (!compiled || !other.compiled) throw std::runtime_error("Regex not compiled");
        return Regex(minimizedDfa->difference(*other.minimizedDfa));
    }

    bool search(const Regex& re, const std::string& text) {
        if (!re.isCompiled()) throw std::runtime_error("Regex not compiled");

        for (size_t i = 0; i < text.size(); ++i) {
            for (size_t len = 1; len <= text.size() - i; ++len) {
                std::string sub = text.substr(i, len);
                if (re.test(sub)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool search(const Regex& re, const std::string& text, std::string& match) {
        if (!re.isCompiled()) throw std::runtime_error("Regex not compiled");

        for (size_t i = 0; i < text.size(); ++i) {
            for (size_t len = 1; len <= text.size() - i; ++len) {
                std::string sub = text.substr(i, len);
                if (re.test(sub)) {
                    match = sub;
                    return true;
                }
            }
        }
        match.clear();
        return false;
    }
}