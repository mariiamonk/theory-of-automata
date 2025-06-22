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
        return minimizedDfa->simulate(text);
    }

    bool Regex::match(const std::string& text) {
        return test(text);
    }

    std::string Regex::toRegex() const {
        return minimizedDfa->toRegex();
    }

    void Regex::printAutomata(const std::string& name) const {
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

    std::vector<std::pair<size_t, size_t>> findGroupBounds(const std::string& pattern) {
        std::vector<std::pair<size_t, size_t>> bounds;
        std::stack<size_t> groupStarts;

        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == '(') {
                if (i > 0 && pattern[i-1] == '<') continue;
                groupStarts.push(i);
            }
            else if (pattern[i] == ')') {
                if (!groupStarts.empty()) {
                    bounds.emplace_back(groupStarts.top(), i);
                    groupStarts.pop();
                }
            }
        }

        return bounds;
    }

    bool search(const std::string& text, Match& match, const Regex& regex) {
        struct StateConfig {
            std::shared_ptr<NFAState> state;
            size_t pos;
            std::map<size_t, size_t> groupStart;
            std::map<size_t, size_t> groupEnd;
            size_t groupCounter = 1;
        };

        auto epsilonClosure = [](const std::set<std::shared_ptr<NFAState>> &states) {
            std::set<std::shared_ptr<NFAState>> closure = states;
            std::stack<std::shared_ptr<NFAState>> stack;
            for (const auto &s: states) stack.push(s);

            while (!stack.empty()) {
                auto state = stack.top();
                stack.pop();
                for (const auto &next: state->epsilonTransitions) {
                    if (closure.insert(next).second) {
                        stack.push(next);
                    }
                }
            }
            return closure;
        };

        auto &nfa = regex.getNFA();
        auto startState = nfa.getStartState();

        for (size_t i = 0; i < text.size(); ++i) {
            std::queue<StateConfig> q;
            std::set<std::shared_ptr<NFAState>> initialClosure = epsilonClosure({startState});

            for (const auto &state: initialClosure) {
                q.push({state, i, {}, {}, 1});
            }

            while (!q.empty()) {
                StateConfig config = q.front();
                q.pop();

                auto state = config.state;
                size_t pos = config.pos;

                if (state->isFinal) {
                    std::vector<std::string> groups;
                    // Группа 0 - полное совпадение
                    groups.push_back(text.substr(i, pos - i));

                    // Добавляем остальные группы в порядке их номеров
                    for (size_t g = 1; g < config.groupCounter; ++g) {
                        if (config.groupStart.count(g) && config.groupEnd.count(g)) {
                            size_t start = config.groupStart.at(g);
                            size_t end = config.groupEnd.at(g);
                            if (start <= end && end <= text.size()) {
                                groups.push_back(text.substr(start, end - start));
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
                for (const auto &next: state->epsilonTransitions) {
                    auto newConfig = config;
                    newConfig.state = next;

                    // Обработка начала группы
                    if (!next->groupStart.empty()) {
                        for (const auto &groupName: next->groupStart) {
                            newConfig.groupStart[newConfig.groupCounter] = pos;
                            newConfig.groupCounter++;
                        }
                    }

                    // Обработка конца группы
                    if (!next->groupEnd.empty()) {
                        for (const auto &groupName: next->groupEnd) {
                            // Ищем последнюю открытую группу с таким именем
                            for (size_t g = newConfig.groupCounter - 1; g >= 1; --g) {
                                if (newConfig.groupStart.count(g) && !newConfig.groupEnd.count(g)) {
                                    newConfig.groupEnd[g] = pos;
                                    break;
                                }
                            }
                        }
                    }

                    q.push(newConfig);
                }

                // Symbol transitions
                if (pos < text.size()) {
                    char c = text[pos];
                    auto it = state->transitions.find(c);
                    if (it != state->transitions.end() || state->transitions.count('.')) {
                        char transitionChar = (it != state->transitions.end()) ? c : '.';
                        for (const auto &next: state->transitions.at(transitionChar)) {
                            auto newConfig = config;
                            newConfig.state = next;
                            newConfig.pos = pos + 1;

                            // Обработка начала группы
                            if (!next->groupStart.empty()) {
                                for (const auto &groupName: next->groupStart) {
                                    newConfig.groupStart[newConfig.groupCounter] = pos;
                                    newConfig.groupCounter++;
                                }
                            }

                            // Обработка конца группы
                            if (!next->groupEnd.empty()) {
                                for (const auto &groupName: next->groupEnd) {
                                    // Ищем последнюю открытую группу с таким именем
                                    for (size_t g = newConfig.groupCounter - 1; g >= 1; --g) {
                                        if (newConfig.groupStart.count(g) && !newConfig.groupEnd.count(g)) {
                                            newConfig.groupEnd[g] = pos + 1;
                                            break;
                                        }
                                    }
                                }
                            }

                            q.push(newConfig);
                        }
                    }
                }
            }
        }

        return false;
    }
}