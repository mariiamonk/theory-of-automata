//#include <iostream>
//#include "Menu/Prog.h"
//void testAutoPrint(){
//    std::cout << "=== Testing min ===" << std::endl;
//        std::string re = ("a+b|a*b|aaab");
//        //std::string re = "a*bcd?";
//        std::cout << "a+b|a*b|aaab" << std::endl;
//        // ("(a*3?&-)|c");
//        AbstractTree::AST ast(re);
//        ast.print();
//        std::cout << std::endl;
//
//        Regex::NFA nfa(ast);
//        nfa.visualizeNFA("png/nfa.png");
//        nfa.print();
//        std::cout << std::endl;
//
//        Regex::DFA dfa(nfa);
//        dfa.visualizeDFA("png/dfa.png");
//        dfa.print();
//        std::cout << std::endl;
//
//        Regex::MinimizedDFA minDfa(dfa);
//        minDfa.print();
//        minDfa.visualize("png/min_dfa.png");
//
//    Regex::Regex r("a+b|a*b|aaab");
//       std::cout << "Recovered regex: " << r.toRegex() << std::endl;
//}

//int main() {
//    try {
//        //testAutoPrint();
////        Regex::Regex re("((<g>reg))(a|b)-c+s?r{2,4}&.<g>");Regex::Match match;
////         Regex::search("asdsfdsfbhvregbcrrrr.regbfhsdbfsbdfsdbf=regbcrr.reg=", match, re);
////         std::cout << match.str() << std::endl;
////         std::cout << match[0] << std::endl;
////
////        Regex::Regex a("a*b");
////        Regex::Regex b("ab+");
////
////        std::cout << "a*b to regex: " << a.toRegex() << std::endl;
////
////        a.printAutomata("a");
////        b.printAutomata("b");
//
////        a.inverse().printAutomata("inv_a-");
////        a.difference(b).printAutomata("dif_a-b-");
//
////        Regex::Regex re("(a(b+)c)");
////        Regex::Match m;
////        search("testabbcxyz", m, re);
////            std::cout << "Full match: " << m.str() << std::endl; // "abbc"
////            std::cout << "Group 1: " << m[1] << std::endl; // "abbc"
////            std::cout << "Group 2: " << m[2] << std::endl; // "bb"
//
//    } catch (const std::exception& e) {
//        std::cerr << "Error: " << e.what() << std::endl;
//        return 1;
//    }
//}

#include "gtest/gtest.h"
int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}