#include "NFA.h"
#include "../AST/Operations.h"
#include "../AST/CharNode.h"
#include <iostream>

namespace Regex {
    using namespace AbstractTree;

    NFA::NFA(const AbstractTree::AST& ast) {
        startState = std::make_shared<NFAState>(stateCounter++);
        endState = std::make_shared<NFAState>(stateCounter++);
        endState->isFinal = true;

        states.push_back(startState);
        states.push_back(endState);

        buildFromAST(ast.getRoot());
    }

    void NFA::buildFromAST(const std::shared_ptr<AbstractTree::ASTNode>& node) {
        processNode(node, startState, endState);
    }

    void NFA::processNode(const std::shared_ptr<AbstractTree::ASTNode>& node,
                          std::shared_ptr<NFAState> start,
                          std::shared_ptr<NFAState> end) {

        if (auto charNode = std::dynamic_pointer_cast<CharNode>(node)) {
            if (charNode->getValue() == '\0') {
                start->epsilonTransitions.insert(end);
            } else {
                start->transitions[charNode->getValue()].insert(end);
            }
        }
        else if (auto anySymbol = std::dynamic_pointer_cast<AnySymbol>(node)) {
            for (char c = 32; c <= 126; c++) {
                start->transitions[c].insert(end);
            }
        }
        else if (auto opNode = std::dynamic_pointer_cast<OperationNode>(node)) {
            const auto& args = opNode->getArguments();

            if (dynamic_cast<Or*>(opNode.get())) {
                auto split = std::make_shared<NFAState>(stateCounter++);
                auto merge = std::make_shared<NFAState>(stateCounter++);
                states.push_back(split);
                states.push_back(merge);

                start->epsilonTransitions.insert(split);
                processNode(args[0], split, merge);
                processNode(args[1], split, merge);
                merge->epsilonTransitions.insert(end);
            }
            else if (dynamic_cast<Concatenation*>(opNode.get())) {
                auto mid = std::make_shared<NFAState>(stateCounter++);
                states.push_back(mid);

                processNode(args[0], start, mid);
                processNode(args[1], mid, end);
            }
            else if (dynamic_cast<KliniClosure*>(opNode.get())) {
                // A*
                auto loop = std::make_shared<NFAState>(stateCounter++);
                auto next = std::make_shared<NFAState>(stateCounter++);

                states.push_back(loop);
                states.push_back(next);

                start->epsilonTransitions.insert(next);

                start->epsilonTransitions.insert(loop);

                processNode(args[0], loop, loop);

                loop->epsilonTransitions.insert(next);

                next->epsilonTransitions.insert(end);
            }
            else if (dynamic_cast<Optional*>(opNode.get())) {
                auto split = std::make_shared<NFAState>(stateCounter++);
                auto merge = std::make_shared<NFAState>(stateCounter++);
                states.push_back(split);
                states.push_back(merge);

                start->epsilonTransitions.insert(split);

                processNode(args[0], split, merge);

                split->epsilonTransitions.insert(merge);

                merge->epsilonTransitions.insert(end);
            }
            else if (dynamic_cast<PositiveClosure*>(opNode.get())) {
                // A+
                auto loopStart = std::make_shared<NFAState>(stateCounter++);
                auto loopEnd = std::make_shared<NFAState>(stateCounter++);

                states.push_back(loopStart);
                states.push_back(loopEnd);

                start->epsilonTransitions.insert(loopStart);

                loopEnd->epsilonTransitions.insert(loopStart);
                loopEnd->epsilonTransitions.insert(end);

                processNode(args[0], loopStart, loopEnd);
            }
            else if (auto group = std::dynamic_pointer_cast<CatchGroup>(opNode)) {
                // Создаём два промежуточных состояния
                auto groupStartState = std::make_shared<NFAState>(stateCounter++);
                auto groupEndState = std::make_shared<NFAState>(stateCounter++);

                groupStartState->groupStart = group->getName();
                groupEndState->groupEnd = group->getName();

                states.push_back(groupStartState);
                states.push_back(groupEndState);

                // Подключаем их к цепочке
                start->epsilonTransitions.insert(groupStartState);
                groupEndState->epsilonTransitions.insert(end);

                processNode(args[0], groupStartState, groupEndState);
            }


        }
    }

    NFA NFA::fromCustomStates(const std::shared_ptr<NFAState>& start, const std::vector<std::shared_ptr<NFAState>>& states) {
        NFA nfa = NFA(AST());
        nfa.startState = start;
        nfa.states = states;
        return nfa;
    }

    void NFA::print() const {
        std::cout << "NFA States:" << std::endl;
        for (const auto& state : states) {
            std::cout << "State " << state->id;
            if (state == startState) std::cout << " (start)";
            if (state->isFinal) std::cout << " (final)";
            std::cout << std::endl;

            for (const auto& trans : state->transitions) {
                std::cout << "  on '" << trans.first << "' to: ";
                for (const auto& target : trans.second) {
                    std::cout << target->id << " ";
                }
                std::cout << std::endl;
            }

            if (!state->epsilonTransitions.empty()) {
                std::cout << "  on ε to: ";
                for (const auto& target : state->epsilonTransitions) {
                    std::cout << target->id << " ";
                }
                std::cout << std::endl;
            }
        }
    }

    void NFA::generateDotFile(const std::string& filename) const{
        std::ofstream out;
        out.open(filename);
        out << "digraph NFA {\n";
        out << "  rankdir=LR;\n";
        out << "  node [shape = circle];\n";

        out << "  node [shape = doublecircle]; " << endState->id << ";\n";
        out << "  node [shape = circle];\n";

        out << "  \"\" [shape = none];\n";
        out << "  \"\" -> " << startState->id << ";\n";

        for (const auto& state : states) {
            for (const auto& trans : state->transitions) {
                for (const auto& target : trans.second) {
                    out << "  " << state->id << " -> " << target->id
                        << " [label=\"" << (trans.first == ' ' ? "space" :
                                            trans.first == '\t' ? "tab" :
                                            trans.first == '\n' ? "\\\\n" :
                                            trans.first == '\r' ? "\\\\r" :
                                            trans.first == '\0' ? "\\\\0" :
                                            std::string(1, trans.first)) << "\"];\n";
                }
            }

            for (const auto& target : state->epsilonTransitions) {
                out << "  " << state->id << " -> " << target->id << " [label=\"ε\"];\n";
            }
        }

        out << "}\n";
        out.close();
    }

    void NFA::visualizeNFA(const std::string& outputFilename = "png/nfa.png") {
        std::string dotFilename = "dot/nfa.dot";
        generateDotFile(dotFilename);

        std::string command = "dot -Tpng " + dotFilename + " -o " + outputFilename;
        int result = system(command.c_str());

        if (result != 0) {
            std::cerr << "Failed to generate NFA visualization. Make sure Graphviz is installed.\n";
        } else {
            std::cout << "NFA visualization saved to " << outputFilename << "\n";
        }
    }

}