#include "MinimizedDFA.h"
#include <sstream>
#include <stack>

namespace Regex {

    MinimizedDFA::MinimizedDFA(const DFA& dfa) {
        checkMemorySafety(dfa.getStates().size());
        minimize(dfa);
    }

    void MinimizedDFA::checkMemorySafety(size_t stateCount) const {
        if (stateCount > MAX_STATES) {
            throw std::runtime_error("The maximum number of states has been exceeded.");
        }
    }

    void MinimizedDFA::minimize(const DFA& dfa) {
        std::vector<std::set<size_t>> partitions;
        initialPartition(dfa, partitions);
        refinePartitions(partitions, dfa);
        buildMinimizedDFA(dfa, partitions);
    }

    void MinimizedDFA::initialPartition(const DFA& dfa, std::vector<std::set<size_t>>& partitions) {
        std::set<size_t> finalStates;
        std::set<size_t> nonFinalStates;

        const auto& dfaStates = dfa.getStates();
        for (size_t i = 0; i < dfaStates.size(); ++i) {
            if (dfaStates[i]->isFinal) {
                finalStates.insert(i);
            } else {
                nonFinalStates.insert(i);
            }
        }

        if (!finalStates.empty()) partitions.push_back(std::move(finalStates));
        if (!nonFinalStates.empty()) partitions.push_back(std::move(nonFinalStates));
    }

    void MinimizedDFA::refinePartitions(std::vector<std::set<size_t>>& partitions, const DFA& dfa) {
        bool changed;
        do {
            changed = false;
            std::vector<std::set<size_t>> newPartitions;

            for (const auto& group : partitions) {
                if (group.size() <= 1) {
                    newPartitions.push_back(group);
                    continue;
                }

                std::map<std::vector<size_t>, std::set<size_t>> subgroups;

                for (size_t stateId : group) {
                    const auto& state = dfa.getStates()[stateId];
                    std::vector<size_t> signature;

                    for (const auto& trans : state->transitions) {
                        size_t targetId = std::distance(
                                dfa.getStates().begin(),
                                std::find(dfa.getStates().begin(), dfa.getStates().end(), trans.second)
                        );
                        for (size_t i = 0; i < partitions.size(); ++i) {
                            if (partitions[i].count(targetId)) {
                                signature.push_back(i);
                                break;
                            }
                        }
                    }
                    subgroups[signature].insert(stateId);
                }
                if (subgroups.size() > 1) {
                    changed = true;
                    for (auto& subgroup : subgroups) {
                        newPartitions.push_back(std::move(subgroup.second));
                    }
                } else {
                    newPartitions.push_back(group);
                }
            }

            partitions = std::move(newPartitions);
        } while (changed);
    }

    void MinimizedDFA::buildMinimizedDFA(const DFA& dfa, const std::vector<std::set<size_t>>& partitions) {
        std::unordered_map<size_t, size_t> stateToPartition;
        for (size_t i = 0; i < partitions.size(); ++i) {
            for (size_t stateId : partitions[i]) {
                stateToPartition[stateId] = i;
            }
        }
        states.resize(partitions.size());
        for (size_t i = 0; i < partitions.size(); ++i) {
            states[i] = std::make_shared<DFAState>();
            // Группа является принимающей, если любое состояние в ней принимающее
            states[i]->isFinal = dfa.getStates()[*partitions[i].begin()]->isFinal;
        }
        size_t startStateId = std::distance(
                dfa.getStates().begin(),
                std::find(dfa.getStates().begin(), dfa.getStates().end(), dfa.getStartState())
        );
        startState = states[stateToPartition[startStateId]];

        for (size_t i = 0; i < partitions.size(); ++i) {
            size_t representativeId = *partitions[i].begin();
            const auto& representativeState = dfa.getStates()[representativeId];

            for (const auto& trans : representativeState->transitions) {
                size_t targetId = std::distance(
                        dfa.getStates().begin(),
                        std::find(dfa.getStates().begin(), dfa.getStates().end(), trans.second)
                );
                states[i]->transitions[trans.first] = states[stateToPartition[targetId]];
            }
        }
    }

    MinimizedDFA MinimizedDFA::complement() const {
        MinimizedDFA result(*this);

        // Глубокая копия состояний
        std::unordered_map<std::shared_ptr<DFAState>, std::shared_ptr<DFAState>> oldToNew;

        result.states.clear();

        // Создание новых состояний с инверсией флага isFinal
        for (const auto& oldState : this->states) {
            auto newState = std::make_shared<DFAState>();
            newState->isFinal = !oldState->isFinal;
            oldToNew[oldState] = newState;
            result.states.push_back(newState);
        }

        // Перенос переходов
        for (size_t i = 0; i < this->states.size(); ++i) {
            const auto& oldState = this->states[i];
            auto& newState = result.states[i];
            for (const auto& [symbol, target] : oldState->transitions) {
                newState->transitions[symbol] = oldToNew.at(target);
            }
        }

        // Установка нового стартового состояния
        result.startState = oldToNew.at(this->startState);

        return result;
    }

    MinimizedDFA MinimizedDFA::intersection(const MinimizedDFA& other) const {
        using StatePair = std::pair<std::shared_ptr<DFAState>, std::shared_ptr<DFAState>>;

        std::map<StatePair, std::shared_ptr<DFAState>> stateMap;
        std::queue<StatePair> queue;

        auto start1 = this->startState;
        auto start2 = other.startState;

        auto start = std::make_shared<DFAState>();
        start->isFinal = start1->isFinal && start2->isFinal;

        MinimizedDFA result(*this);  // Создаем временную копию для заготовки
        result.states.clear();
        result.startState = start;

        stateMap[{start1, start2}] = start;
        result.states.push_back(start);
        queue.push({start1, start2});

        while (!queue.empty()) {
            auto [s1, s2] = queue.front();
            queue.pop();
            auto curr = stateMap[{s1, s2}];

            // Объединяем алфавиты обоих автоматов
            std::set<char> symbols;
            for (const auto& [c, _] : s1->transitions) symbols.insert(c);
            for (const auto& [c, _] : s2->transitions) symbols.insert(c);

            for (char symbol : symbols) {
                auto it1 = s1->transitions.find(symbol);
                auto it2 = s2->transitions.find(symbol);
                if (it1 != s1->transitions.end() && it2 != s2->transitions.end()) {
                    auto t1 = it1->second;
                    auto t2 = it2->second;
                    StatePair nextPair = {t1, t2};

                    if (stateMap.find(nextPair) == stateMap.end()) {
                        auto newState = std::make_shared<DFAState>();
                        newState->isFinal = t1->isFinal && t2->isFinal;
                        stateMap[nextPair] = newState;
                        result.states.push_back(newState);
                        queue.push(nextPair);
                    }

                    curr->transitions[symbol] = stateMap[nextPair];
                }
            }
        }

        return result;
    }


    MinimizedDFA MinimizedDFA::difference(const MinimizedDFA& other) const {
        MinimizedDFA complementB = other.complement();
        return this->intersection(complementB);
    }

    void MinimizedDFA::print() const {
        std::cout << "Minimized DFA States:" << std::endl;
        for (size_t i = 0; i < states.size(); ++i) {
            std::cout << "State " << i;
            if (states[i] == startState) std::cout << " (start)";
            if (states[i]->isFinal) std::cout << " (final)";
            std::cout << std::endl;

            for (const auto &trans: states[i]->transitions) {
                size_t targetIndex = std::find(states.begin(), states.end(), trans.second) - states.begin();
                std::cout << "  on '" << trans.first << "' to: " << targetIndex << std::endl;
            }
        }
    }


    bool MinimizedDFA::simulate(const std::string& input) const {
        auto current = startState;

        for (char c : input) {
            auto it = current->transitions.find(c);
            if (it == current->transitions.end()) return false;
            current = it->second;
        }

        return current->isFinal;
    }

    void MinimizedDFA::visualize(const std::string& outputFilename = "min_dfa.png") const {
        std::ofstream dot("min_dfa.dot");
        if (!dot) {
            std::cerr << "Error creating DOT file\n";
            return;
        }
        dot << "digraph MinDFA {\n"
            << "  rankdir=LR;\n"
            << "  node [shape=circle];\n";

        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->isFinal) {
                dot << "  " << i << " [shape=doublecircle];\n";
            }
        }

        size_t startIdx = std::find(states.begin(), states.end(), startState) - states.begin();
        dot << "  invisible [shape=point, width=0];\n"
            << "  invisible -> " << startIdx << ";\n";

        for (size_t i = 0; i < states.size(); ++i) {
            for (const auto& [symbol, target] : states[i]->transitions) {
                auto target_pos = std::find(states.begin(), states.end(), target) - states.begin();
                dot << "  " << i << " -> " << target_pos
                    << " [label=\"";

                if (symbol == ' ') dot << "' '";
                else if (symbol == '\t') dot << "\\\\t";
                else if (symbol == '\n') dot << "\\\\n";
                else if (symbol < 32 || symbol > 126)
                    dot << "0x" << std::hex << (int)symbol;
                else dot << symbol;

                dot << "\"];\n";
            }
        }

        dot << "}\n";
        dot.close();

        std::string cmd = "dot -Tpng min_dfa.dot -o " + outputFilename;
        if (system(cmd.c_str()) != 0) {
            std::cerr << "Graphviz error. Check if 'dot' is installed.\n";
        } else {
            std::cout << "Visualization saved to " << outputFilename << "\n";
        }
    }
}

namespace Regex {

    std::string MinimizedDFA::toRegex(int k) const {
        size_t n = states.size();
        std::vector<std::vector<std::string>> R(n, std::vector<std::string>(n));

        for (size_t i = 0; i < n; ++i) {
            for (const auto& [symbol, target] : states[i]->transitions) {
                size_t j = std::distance(states.begin(), std::find(states.begin(), states.end(), target));
                std::string s(1, symbol);
                if (!R[i][j].empty()) {
                    R[i][j] = "(" + R[i][j] + "|" + s + ")";
                } else {
                    R[i][j] = s;
                }
            }
        }

        for (size_t k = 0; k < n; ++k) {
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    if (R[i][k].empty() || R[k][j].empty()) continue;

                    std::string part = "(" + R[i][k] + ")";
                    if (!R[k][k].empty()) {
                        part += "(" + R[k][k] + ")*";
                    }
                    part += "(" + R[k][j] + ")";

                    if (!R[i][j].empty()) {
                        R[i][j] = "(" + R[i][j] + ")|(" + part + ")";
                    } else {
                        R[i][j] = part;
                    }
                }
            }
        }

        size_t startIndex = std::distance(states.begin(), std::find(states.begin(), states.end(), startState));
        std::string result;

        for (size_t i = 0; i < n; ++i) {
            if (states[i]->isFinal && !R[startIndex][i].empty()) {
                if (!result.empty()) result += "|";
                result += "(" + R[startIndex][i] + ")";
            }
        }

        return result.empty() ? "∅" : simplifyRegex(result);
    }



    std::string MinimizedDFA::simplifyRegex(const std::string& regex) const {
        if (regex.empty()) return "";

        std::string result = regex;
        bool changed;

        do {
            changed = false;

            if (result.size() > 2 && result[0] == '(' && result.back() == ')') {
                bool can_remove = true;
                int balance = 0;
                for (size_t i = 1; i < result.size() - 1; ++i) {
                    if (result[i] == '(') balance++;
                    if (result[i] == ')') balance--;
                    if (balance < 0 || (result[i] == '|' && balance == 0)) {
                        can_remove = false;
                        break;
                    }
                }
                if (can_remove) {
                    result = result.substr(1, result.size() - 2);
                    changed = true;
                }
            }

            size_t pos;
            while ((pos = result.find("(())")) != std::string::npos) {
                result.replace(pos, 4, "");
                changed = true;
            }

            if ((pos = result.find(")|(")) != std::string::npos) {
                if (result.size() > 4 && result[0] == '(' && result.back() == ')') {
                    std::string inner = result.substr(1, result.size() - 2);
                    if (inner.find(")|(") == std::string::npos ||
                        inner.find('(') == std::string::npos) {
                        result = inner;
                        changed = true;
                    }
                }
            }
        } while (changed);

        return result;
    }
}