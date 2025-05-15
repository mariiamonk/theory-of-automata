#ifndef DFA_H
#define DFA_H
#include "../NFA/NFA.h"
#include <set>
#include <map>
#include <vector>
#include <memory>
namespace Regex {
    class DFAState {
    public:
        std::map<char, std::shared_ptr<DFAState>> transitions;
        bool isFinal = false;
        std::set<size_t> nfaStates;
    };
    class DFA {
    private:
        std::shared_ptr<DFAState> startState;
        std::vector<std::shared_ptr<DFAState>> states;

        void buildFromNFA(const NFA& nfa);

        std::set<size_t> epsilonClosure(const NFA& nfa, const std::set<size_t>& states);
        std::set<size_t> move(const NFA& nfa, const std::set<size_t>& states, char c);
    public:
        explicit DFA(const NFA& nfa);
        DFA(std::shared_ptr<DFAState> start, const std::vector<std::shared_ptr<DFAState>>& states): startState(std::move(start)), states(states) {}

        [[nodiscard]] std::shared_ptr<DFAState> getStartState() const { return startState; }
        [[nodiscard]] const std::vector<std::shared_ptr<DFAState>>& getStates() const { return states; }

        void print() const;
        void visualizeDFA(const std::string&);

        [[nodiscard]] bool simulate(const std::string& input) const;
    };
}
#endif // DFA_H