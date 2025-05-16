#include <iostream>
#include "Menu/Prog.h"
void testAutoPrint(){
    std::cout << "=== Testing min ===" << std::endl;
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

    Regex::Regex r("a+b|a*b|aaab");
       std::cout << "Recovered regex: " << r.toRegex() << std::endl;
}

int main() {
    try {
        //testAutoPrint();
         Regex::Regex re("((<g>reg))(a|b)-c+s?r{2,4}&.<g>");
         re.printAutomata("first-");

         std::cout <<"((<g>reg))(a|b)-c+s?r{2,4}&.<g> in asdsfdsfbhvregbcrr.regbfhsdbfsbdfsdbf"<< ((Regex::search(re, "asdsfdsfbhvregbcrr.regbfhsdbfsbdfsdbf")) ? " found" : " not found") << std::endl;
         std::string match;
         Regex::search(re, "asdsfdsfbhvregbcrrDregbfhsdbfsbdfsdbf=regbcrr.reg=", match);
         std::cout << match << std::endl;

        Regex::Regex a("a*b");
        Regex::Regex b("ab+");

        std::cout << "a*b to regex: " << a.toRegex() << std::endl;

        a.printAutomata("a");
        b.printAutomata("b");

        a.inverse().printAutomata("inv_a-");
        a.difference(b).printAutomata("dif_a-b-");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}