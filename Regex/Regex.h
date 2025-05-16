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

    class Regex {
    private:
        std::string pattern;
        std::unique_ptr<AbstractTree::AST> ast;
        std::unique_ptr<NFA> nfa;
        std::unique_ptr<DFA> dfa;
        std::unique_ptr<MinimizedDFA> minimizedDfa;
        bool compiled = false;

    public:
        explicit Regex(std::string  pattern);

        void compile();

        Regex(MinimizedDFA&& mdfa) : pattern(""), compiled(true), ast(), nfa(), dfa(){
            minimizedDfa = std::make_unique<MinimizedDFA>(std::move(mdfa));
        }

        bool isCompiled() const { return compiled; }

        Regex(const Regex&) = delete;
        Regex& operator=(const Regex&) = delete;

        Regex(Regex&&) = default;
        Regex& operator=(Regex&&) = default;

        [[nodiscard]] bool test(const std::string& text) const;
        [[nodiscard]] std::string toRegex() const;

        void printAutomata(const std::string& name) const;

        [[nodiscard]] Regex inverse() const;
        [[nodiscard]] Regex difference(const Regex& other) const;

        bool match(const std::string& text);
    };

    bool search(const Regex& re, const std::string& text);
    bool search(const Regex& re, const std::string& text, std::string& match);

} // namespace Regex

#endif // REGEX_H