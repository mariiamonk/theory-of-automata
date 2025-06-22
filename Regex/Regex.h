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

    class Match {
    private:
        std::string fullMatch;
        std::vector<std::string> groups;

    public:
        Match() = default;

        void set(const std::string& full, std::vector<std::string> captures) {
            fullMatch = full;
            groups = std::move(captures);
        }

        const std::string& str() const { return fullMatch; }
        const std::string& operator[](size_t i) const {
            if (i >= groups.size()) throw std::out_of_range("Match group index out of range");
            return groups[i];
        }
        size_t size() const { return groups.size(); }


        std::vector<std::string>::const_iterator begin() const { return groups.begin(); }
        std::vector<std::string>::const_iterator end() const { return groups.end(); }
    };


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
        NFA& getNFA() const {
            if (!compiled || !nfa) {
                throw std::runtime_error("Regex not compiled or NFA not available");
            }
            return *nfa;
        }

        Regex(const Regex&) = default;
        Regex& operator=(const Regex&) = delete;

        Regex(Regex&&) = default;
        Regex& operator=(Regex&&) = default;

        [[nodiscard]] bool test(const std::string& text) const;
        std::string toRegex() const;

        void printAutomata(const std::string& name) const;

        [[nodiscard]] Regex inverse() const;
        [[nodiscard]] Regex difference(const Regex& other) const;
        const std::unique_ptr<AbstractTree::AST>& getAST() const {
            if (!compiled || !ast) {
                throw std::runtime_error("Regex not compiled or AST not available");
            }
            return ast;
        }
        const std::string getPattern() const{
            return pattern;
        }

        bool match(const std::string& text);
    };

    bool search(const Regex& re, const std::string& text);
    bool search(const std::string& text, Match& match, const Regex& regex);

} // namespace Regex

#endif // REGEX_H