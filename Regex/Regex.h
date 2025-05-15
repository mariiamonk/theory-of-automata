#ifndef REGEX_H
#define REGEX_H

#include "../AST/AST.h"
#include "../NFA/NFA.h"
#include "../DFA/DFA.h"
#include "../DFA/MinimizedDFA.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Regex {

    class MatchResult {
    public:
        bool success;
        std::string match;
        std::map<std::string, std::string> groups;

        explicit operator bool() const { return success; }
    };

    class Regex {
    private:
        std::string pattern;
        std::unique_ptr<AbstractTree::AST> ast;
        std::unique_ptr<NFA> nfa;
        std::unique_ptr<DFA> dfa;
        std::unique_ptr<MinimizedDFA> minimizedDfa;
        bool compiled = false;

        void compile();

    public:
        explicit Regex(const std::string& pattern);

        Regex(MinimizedDFA&& mdfa) : pattern(""), compiled(true) {
            minimizedDfa = std::make_unique<MinimizedDFA>(std::move(mdfa));
        }

        Regex(const Regex&) = delete;
        Regex& operator=(const Regex&) = delete;

        Regex(Regex&&) = default;
        Regex& operator=(Regex&&) = default;

        MatchResult search(const std::string& text) const;

        bool test(const std::string& text) const;
        std::string toRegex() const;

        void printAutomata() const;

        Regex inverse() const;
        Regex difference(const Regex& other) const;

        bool match(const std::string& text);
    };

} // namespace Regex

#endif // REGEX_H