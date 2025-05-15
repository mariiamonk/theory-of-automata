#ifndef NFA_H
#define NFA_H

#include <memory>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "../AST/AST.h"

namespace Regex {

    class NFAState {
    public:
        std::map<char, std::set<std::shared_ptr<NFAState>>> transitions;
        std::set<std::shared_ptr<NFAState>> epsilonTransitions;
        bool isFinal = false;
        size_t id;

        std::string groupStart;
        std::string groupEnd;

        explicit NFAState(size_t id) : id(id) {}
    };
    class NFA {
    private:
        std::shared_ptr<NFAState> startState;
        std::shared_ptr<NFAState> endState;
        std::vector<std::shared_ptr<NFAState>> states;
        size_t stateCounter = 0;

        void buildFromAST(const std::shared_ptr<AbstractTree::ASTNode>& node);
        void processNode(const std::shared_ptr<AbstractTree::ASTNode>& node, std::shared_ptr<NFAState> start, std::shared_ptr<NFAState> end);

    public:
        explicit NFA(const AbstractTree::AST& ast);

        [[nodiscard]] std::shared_ptr<NFAState> getStartState() const { return startState; }
        [[nodiscard]] std::shared_ptr<NFAState> getEndState() const { return endState; }
        [[nodiscard]] const std::vector<std::shared_ptr<NFAState>>& getStates() const { return states; }

        static NFA fromCustomStates(const std::shared_ptr<NFAState>& start, const std::vector<std::shared_ptr<NFAState>>& states);

        void generateDotFile(const std::string& filename) const;
        void visualizeNFA(const std::string& outputFilename);

        void print() const;
    };

}
#endif // NFA_H