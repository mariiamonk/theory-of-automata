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

        void initialPartition(const DFA& dfa, std::vector<std::set<size_t>>& partitions);
        void refinePartitions(std::vector<std::set<size_t>>& partitions, const DFA& dfa);
        void buildMinimizedDFA(const DFA& dfa, const std::vector<std::set<size_t>>& partitions);

        void checkMemorySafety(size_t stateCount) const;

    public:
        explicit MinimizedDFA(const DFA& dfa);

        std::shared_ptr<DFAState> getStartState() const { return startState; }
        const std::vector<std::shared_ptr<DFAState>>& getStates() const { return states; }

        MinimizedDFA complement() const;
        MinimizedDFA intersection(const MinimizedDFA& other) const;
        MinimizedDFA difference(const MinimizedDFA& other) const;

        void print() const;
        void visualize(const std::string& outputFilename) const;
        bool simulate(const std::string& input) const;
        std::string toRegex(int k = -1) const;

        void minimize(const DFA& dfa);
        std::string simplifyRegex(const std::string& regex) const;
    };
}

#endif // MINIMIZED_DFA_H