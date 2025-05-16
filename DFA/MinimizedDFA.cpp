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

    void MinimizedDFA::visualize(const std::string& outputFilename = "png/min_dfa.png") const {
        std::ofstream dot("dot/min_dfa.dot");
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

        std::string cmd = "dot -Tpng dot/min_dfa.dot -o " + outputFilename;
        if (system(cmd.c_str()) != 0) {
            std::cerr << "Graphviz error. Check if 'dot' is installed.\n";
        } else {
            std::cout << "Visualization saved to " << outputFilename << "\n";
        }
    }
}

namespace Regex {

    std::string MinimizedDFA::toRegex(int) const {
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



    std::string MinimizedDFA::simplifyRegex(const std::string& regex) {
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

    MinimizedDFA MinimizedDFA::reverseLanguage() const {
        using namespace Regex;

        std::vector<std::shared_ptr<NFAState>> reversedStates(states.size());
        for (size_t i = 0; i < states.size(); ++i) {
            reversedStates[i] = std::make_shared<NFAState>(i);
        }

        for (size_t i = 0; i < states.size(); ++i) {
            for (const auto& [symbol, target] : states[i]->transitions) {
                size_t targetIdx = std::distance(states.begin(), std::find(states.begin(), states.end(), target));
                reversedStates[targetIdx]->transitions[symbol].insert(reversedStates[i]);
            }
        }

        size_t oldStartIdx = std::distance(states.begin(), std::find(states.begin(), states.end(), startState));
        reversedStates[oldStartIdx]->isFinal = true;

        auto newStart = std::make_shared<NFAState>(states.size());
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->isFinal) {
                newStart->epsilonTransitions.insert(reversedStates[i]);
            }
        }

        std::vector<std::shared_ptr<NFAState>> allStates = reversedStates;
        allStates.push_back(newStart);

        NFA reversedNFA = NFA::fromCustomStates(newStart, allStates);

        DFA reversedDFA(reversedNFA);
        return MinimizedDFA(reversedDFA);
    }

    MinimizedDFA MinimizedDFA::difference(const MinimizedDFA& other) const {
        using namespace Regex;

        using StatePair = std::pair<std::shared_ptr<DFAState>, std::shared_ptr<DFAState>>;
        std::map<StatePair, std::shared_ptr<DFAState>> productStates;
        std::queue<StatePair> queue;
        std::vector<std::shared_ptr<DFAState>> allStates;

        auto trap = std::make_shared<DFAState>();
        for (char c = 0; c < 127; ++c) {
            trap->transitions[c] = trap;
        }

        auto startPair = std::make_pair(startState, other.startState);
        auto start = std::make_shared<DFAState>();
        start->isFinal = startState->isFinal && !other.startState->isFinal;

        productStates[startPair] = start;
        queue.push(startPair);
        allStates.push_back(start);

        while (!queue.empty()) {
            auto [s1, s2] = queue.front();
            queue.pop();
            auto current = productStates[{s1, s2}];

            for (const auto& [symbol, t1] : s1->transitions) {
                std::shared_ptr<DFAState> t2;
                if (s2->transitions.count(symbol)) {
                    t2 = s2->transitions.at(symbol);
                } else {
                    t2 = trap;
                }

                auto key = std::make_pair(t1, t2);
                if (!productStates.count(key)) {
                    auto newState = std::make_shared<DFAState>();
                    newState->isFinal = t1->isFinal && !t2->isFinal;
                    productStates[key] = newState;
                    queue.push(key);
                    allStates.push_back(newState);
                }

                current->transitions[symbol] = productStates[key];
            }
        }

        for (auto& [_, state] : productStates) {
            for (auto& [sym, target] : state->transitions) {
                if (target == trap && std::find(allStates.begin(), allStates.end(), trap) == allStates.end()) {
                    allStates.push_back(trap);
                    break;
                }
            }
        }

        DFA resultDFA(start, allStates);
        return MinimizedDFA(resultDFA);
    }
}