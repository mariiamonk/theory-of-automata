#ifndef MINIMIZED_DFA_H
#define MINIMIZED_DFA_H

#include "DFA.h"
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

namespace Regex {
    class MinimizedDFA {
    private:
        const size_t MAX_STATES = 1000;
        std::shared_ptr<DFAState> startState;
        std::vector<std::shared_ptr<DFAState>> states;
        std::map<int, int> stateMapping;

        static void initialPartition(const DFA& dfa, std::vector<std::set<size_t>>& partitions);
        static void refinePartitions(std::vector<std::set<size_t>>& partitions, const DFA& dfa);
        void buildMinimizedDFA(const DFA& dfa, const std::vector<std::set<size_t>>& partitions);

        void checkMemorySafety(size_t stateCount) const;

    public:
        explicit MinimizedDFA(const DFA& dfa);

        [[nodiscard]] std::shared_ptr<DFAState> getStartState() const { return startState; }
        [[nodiscard]] const std::vector<std::shared_ptr<DFAState>>& getStates() const { return states; }

        [[nodiscard]] MinimizedDFA reverseLanguage() const;
        [[nodiscard]] MinimizedDFA difference(const MinimizedDFA& other) const;

        void print() const;
        void visualize(const std::string& outputFilename) const;
        [[nodiscard]] bool simulate(const std::string& input) const;
        [[nodiscard]] std::string toRegex(int k = -1) const;

        void minimize(const DFA& dfa);
        [[nodiscard]] static std::string simplifyRegex(const std::string& regex) ;
    };
}

#endif // MINIMIZED_DFA_H