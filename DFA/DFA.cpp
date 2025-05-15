#include "DFA.h"
#include <queue>
#include <algorithm>
#include <unordered_map>

namespace Regex {
    DFA::DFA(const NFA &nfa) {
        buildFromNFA(nfa);
    }

    void DFA::buildFromNFA(const NFA &nfa) {
        std::set<size_t> startClosure = epsilonClosure(nfa,{nfa.getStartState()->id});

        startState = std::make_shared<DFAState>();
        startState->nfaStates = startClosure;
        states.push_back(startState);

        std::queue<std::shared_ptr<DFAState>> unprocessedStates;
        unprocessedStates.push(startState);

        std::map<std::set<size_t>, std::shared_ptr<DFAState>>
                processedStates;
        processedStates[startClosure] = startState;
        while (!unprocessedStates.empty()) {
            auto currentState = unprocessedStates.front();
            unprocessedStates.pop();
            std::set<char> alphabet;
            for (size_t nfaStateId: currentState->nfaStates) {
                const auto &nfaState = nfa.getStates()[nfaStateId];
                for (const auto &trans: nfaState->transitions) {
                    alphabet.insert(trans.first);
                }
            }
            for (char c: alphabet) {
                std::set<size_t> newStateNfa = epsilonClosure(nfa,move(nfa, currentState->nfaStates, c));

                if (newStateNfa.empty()) continue;
                auto it = processedStates.find(newStateNfa);
                if (it == processedStates.end()) {
                    auto newState = std::make_shared<DFAState>();
                    newState->nfaStates = newStateNfa;
                    for (size_t nfaStateId: newStateNfa) {
                        if (nfa.getStates()[nfaStateId]->isFinal) {
                            newState->isFinal = true;
                            break;
                        }
                    }
                    states.push_back(newState);
                    processedStates[newStateNfa] = newState;
                    unprocessedStates.push(newState);
                    currentState->transitions[c] = newState;
                } else {
                    currentState->transitions[c] = it->second;
                }
            }
        }
    }

    std::set<size_t> DFA::epsilonClosure(const NFA &nfa, const
    std::set<size_t> &inputStates) {
        std::set<size_t> closure = inputStates;
        std::queue<size_t> unprocessedStates;
        for (size_t stateId: inputStates) {
            unprocessedStates.push(stateId);
        }
        while (!unprocessedStates.empty()) {
            size_t currentId = unprocessedStates.front();
            unprocessedStates.pop();
            const auto &currentState = nfa.getStates()[currentId];

            for (const auto &target:
                    currentState->epsilonTransitions) {
                if (closure.insert(target->id).second) {
                    unprocessedStates.push(target->id);
                }
            }
        }
        return closure;
    }

    std::set<size_t> DFA::move(const NFA &nfa, const
    std::set<size_t> &states, char c) {
        std::set<size_t> result;
        for (size_t stateId: states) {
            const auto &state = nfa.getStates()[stateId];
            auto it = state->transitions.find(c);
            if (it != state->transitions.end()) {
                for (const auto &target: it->second) {
                    result.insert(target->id);
                }
            }
        }
        return result;
    }

    void DFA::print() const {
        std::cout << "DFA States:" << std::endl;
        for (size_t i = 0; i < states.size(); ++i) {
            std::cout << "State " << i;
            if (states[i] == startState) std::cout << " (start)";
            if (states[i]->isFinal) std::cout << " (final)";
            std::cout << std::endl;
            std::cout << " NFA states: ";
            for (size_t nfaState: states[i]->nfaStates) {
                std::cout << nfaState << " ";
            }
            std::cout << std::endl;
            for (const auto &trans: states[i]->transitions) {
                size_t targetIndex = std::find(states.begin(),
                                               states.end(), trans.second) - states.begin();
                std::cout << " on '" << trans.first << "' to: " <<
                          targetIndex << std::endl;
            }
        }
    }

    bool DFA::simulate(const std::string &input) const {
        auto currentState = startState;
        for (char c: input) {
            auto it = currentState->transitions.find(c);
            if (it == currentState->transitions.end()) {
                return false;
            }
            currentState = it->second;
        }
        return currentState->isFinal;
    }

    void DFA::visualizeDFA(const std::string& outputFilename = "png/dfa.png") {
        std::ofstream dotFile("dot/dfa.dot");
        if (!dotFile.is_open()) {
            std::cerr << "Failed to create DOT file\n";
            return;
        }

        std::unordered_map<std::shared_ptr<DFAState>, size_t> stateIndices;
        size_t currentIndex = 0;

        for (const auto &state: getStates()) {
            stateIndices[state] = currentIndex++;
        }

        dotFile << "digraph DFA {\n";
        dotFile << "  rankdir=LR;\n";
        dotFile << "  node [shape = circle];\n";

        for (const auto &state: getStates()) {
            if (state->isFinal) {
                dotFile << "  node [shape = doublecircle]; " << stateIndices[state] << ";\n";
            }
        }
        dotFile << "  node [shape = circle];\n";

        dotFile << "  \"\" [shape = none];\n";
        dotFile << "  \"\" -> " << stateIndices[getStartState()] << ";\n";

        for (const auto &state: getStates()) {
            for (const auto &trans: state->transitions) {
                char c = trans.first;
                std::string label;

                switch (c) {
                    case ' ':
                        label = "' '";
                        break;
                    case '\t':
                        label = "\\\\t";
                        break;
                    case '\n':
                        label = "\\\\n";
                        break;
                    case '\r':
                        label = "\\\\r";
                        break;
                    case '\0':
                        label = "\\\\0";
                        break;
                    case '\\':
                        label = "\\\\";
                        break;
                    case '"':
                        label = "\\\"";
                        break;
                    default:
                        if (c < 32 || c > 126) {
                            const char hex[] = "0123456789ABCDEF";
                            label = "0x";
                            label += hex[(c >> 4) & 0xF];
                            label += hex[c & 0xF];
                        } else {
                            label = std::string(1, c);
                        }
                }

                dotFile << "  " << stateIndices[state] << " -> " << stateIndices[trans.second]
                        << " [label=\"" << label << "\"];\n";
            }
        }

        dotFile << "}\n";
        dotFile.close();

        std::string command = "dot -Tpng dot/dfa.dot -o " + outputFilename;
        int result = std::system(command.c_str());

        if (result != 0) {
            std::cerr << "Failed to generate DFA image. Make sure Graphviz is installed.\n";
        } else {
            std::cout << "DFA visualization saved to " << outputFilename << "\n";
        }
    }
}
