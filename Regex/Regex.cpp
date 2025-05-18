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

    bool search(const std::string& text, Match& match, const Regex& regex) {
        struct StateConfig {
            std::shared_ptr<NFAState> state;
            size_t pos;
            std::map<std::string, size_t> groupStart;
            std::map<std::string, size_t> groupEnd;
        };

        auto epsilonClosure = [](const std::set<std::shared_ptr<NFAState>>& states) {
            std::set<std::shared_ptr<NFAState>> closure = states;
            std::stack<std::shared_ptr<NFAState>> stack;
            for (const auto& s : states) stack.push(s);

            while (!stack.empty()) {
                auto state = stack.top();
                stack.pop();
                for (const auto& next : state->epsilonTransitions) {
                    if (closure.insert(next).second) {
                        stack.push(next);
                    }
                }
            }
            return closure;
        };

        auto& nfa = regex.getNFA();
        auto startState = nfa.getStartState();

        for (size_t i = 0; i < text.size(); ++i) {
            std::queue<StateConfig> q;
            std::set<std::shared_ptr<NFAState>> initialClosure = epsilonClosure({startState});

            for (const auto& state : initialClosure) {
                q.push({state, i, {}, {}});
            }

            while (!q.empty()) {
                StateConfig config = q.front();
                q.pop();

                auto state = config.state;
                size_t pos = config.pos;

                if (state->isFinal) {
                    std::vector<std::string> groups;

                    for (const auto& [name, startPos] : config.groupStart) {
                        if (config.groupEnd.count(name)) {
                            size_t endPos = config.groupEnd.at(name);
                            if (endPos > startPos && endPos <= text.size()) {
                                groups.push_back(text.substr(startPos, endPos - startPos));
                            } else {
                                groups.push_back("");
                            }
                        } else {
                            groups.push_back("");
                        }
                    }

                    match.set(text.substr(i, pos - i), std::move(groups));
                    return true;
                }

                // Epsilon transitions
                for (const auto& next : state->epsilonTransitions) {
                    auto newStart = config.groupStart;
                    auto newEnd = config.groupEnd;

                    if (!next->groupStart.empty() && newStart.count(next->groupStart) == 0) {
                        newStart[next->groupStart] = pos;
                    }
                    if (!next->groupEnd.empty() && newEnd.count(next->groupEnd) == 0) {
                        newEnd[next->groupEnd] = pos;
                    }

                    q.push({next, pos, std::move(newStart), std::move(newEnd)});
                }

                // Symbol transitions
                if (pos < text.size()) {
                    char c = text[pos];
                    auto it = state->transitions.find(c);
                    if (it != state->transitions.end()) {
                        for (const auto& next : it->second) {
                            auto newStart = config.groupStart;
                            auto newEnd = config.groupEnd;

                            if (!next->groupStart.empty() && newStart.count(next->groupStart) == 0) {
                                newStart[next->groupStart] = pos;
                            }
                            if (!next->groupEnd.empty() && newEnd.count(next->groupEnd) == 0) {
                                newEnd[next->groupEnd] = pos + 1;
                            }

                            q.push({next, pos + 1, std::move(newStart), std::move(newEnd)});
                        }
                    }
                }
            }
        }
        return false;
    }

}