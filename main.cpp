#include <iostream>
#include "Menu/Prog.h"
void testAutoPrint(){
    std::cout << "=== Testing print ast, nfa, dfa ===" << std::endl;
//    {
//        std::string re = (".");
//        AbstractTree::AST ast(re);
//        ast.print();
//        std::cout << std::endl;
//    }
        std::string re = ("a+b|a*b|aaab");
        //std::string re = "a*bcd?";
        std::cout << "a+b|a*b|aaab" << std::endl;
        // ("(a*3?&-)|c");
        AbstractTree::AST ast(re);
        ast.print();
        std::cout << std::endl;

        Regex::NFA nfa(ast);
        nfa.visualizeNFA("png/nfa.png");
        nfa.print();
        std::cout << std::endl;

        Regex::DFA dfa(nfa);
        dfa.visualizeDFA("png/dfa.png");
        dfa.print();
        std::cout << std::endl;

        Regex::MinimizedDFA minDfa(dfa);
        minDfa.print();
        minDfa.visualize("png/min_dfa.png");
//        std::cout << std::endl;
//        Regex::Regex r("a+b|a*b|aaab");
        std::cout << "inv" << std::endl;
    Regex::MinimizedDFA resinv = minDfa.reverseLanguage();

        resinv.print();
        resinv.visualize("png/min_dfa_inv.png");

        std::cout << "dif" << std::endl;
        AbstractTree::AST ast2("bcds");
        Regex::NFA nfa2(ast2);
        Regex::DFA dfa2(nfa2);
        Regex::MinimizedDFA mind2(dfa2);
    Regex::MinimizedDFA resdif = minDfa.difference(mind2);

    resdif.print();
    resdif.visualize("png/min_dfa_dif.png");

    Regex::Regex r("a+b|a*b|aaab");
       std::cout << "Recovered regex: " << r.toRegex() << std::endl;
}

int main() {
    try {
         Regex::Regex re("((<g>reg))(a|b)-c+s?r{2,4}<g>");
         re.printAutomata();
        bool result = re.match("regbcrrreg");
        std::cout << (result ? "YES" : "NO") << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}