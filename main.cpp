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
        //std::string re = ("a+b|a*b|aaab");
        std::string re = "a*bcd?";
        std::cout << "a+b|a*b|aaab" << std::endl;
        // ("(a*3?&-)|c");
        AbstractTree::AST ast(re);
        ast.print();
        std::cout << std::endl;

        Regex::NFA nfa(ast);
        nfa.visualizeNFA("nfa.png");
        nfa.print();
        std::cout << std::endl;

        Regex::DFA dfa(nfa);
        dfa.visualizeDFA("dfa.png");
        dfa.print();
        std::cout << std::endl;

        Regex::MinimizedDFA minDfa(dfa);
        minDfa.print();
        minDfa.visualize("min_nfa.png");
//        std::cout << std::endl;
//        Regex::Regex r("a+b|a*b|aaab");
        std::cout << "complement" << std::endl;
    Regex::MinimizedDFA resinv = minDfa.complement();

        resinv.print();
        resinv.visualize("min_nfa_com.png");
    minDfa.visualize("min_nfa.png");

        std::cout << "dif" << std::endl;
        AbstractTree::AST ast2("bcds");
        Regex::NFA nfa2(ast2);
        Regex::DFA dfa2(nfa2);
        Regex::MinimizedDFA mind2(dfa2);
    Regex::MinimizedDFA resdif = minDfa.difference(mind2);

    resdif.print();
    resdif.visualize("min_nfa_dif.png");

//        std::cout << "Recovered regex: " << r.toRegex(-1) << std::endl;
}
void testBasicOperations() {
    std::cout << "=== Testing basic operations ===" << std::endl;

    // Test OR operation
    {
        Regex::Regex re("a+b|a*b|aaab");
        re.printAutomata();
        auto result = re.search("aab");
        std::cout << "Test 'a+b|a*b|aaab' on 'aab': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("b");
        std::cout << "Test 'a|b' on 'b': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("c");
        std::cout << "Test 'a|b' on 'c': " << (result ? "Found" : "Not found") << std::endl;
    }

    // Test concatenation
    {
        Regex::Regex re("ab-cd"); // Using optional '-' for concatenation
        auto result = re.search("abcd");
        std::cout << "Test 'ab-cd' on 'abcd': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("ab cd");
        std::cout << "Test 'ab-cd' on 'ab cd': " << (result ? "Found" : "Not found") << std::endl;
    }

    // Test positive closure
    {
        Regex::Regex re("a+");
        auto result = re.search("a");
        std::cout << "Test 'a+' on 'a': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("aaaa");
        std::cout << "Test 'a+' on 'aaaa': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("b");
        std::cout << "Test 'a+' on 'b': " << (result ? "Found" : "Not found") << std::endl;
    }

    // Test optional
    {
        Regex::Regex re("a?b");
//        Regex::DFA dfa(Regex::NFA(AbstractTree::AST("a?b")));
//        dfa.print();

        auto result = re.search("ab");
        std::cout << "Test 'a?b' on 'ab': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("b");
        std::cout << "Test 'a?b' on 'b': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("a");
        std::cout << "Test 'a?b' on 'a': " << (result ? "Found" : "Not found") << std::endl;
    }

    // Test any symbol
    {
        Regex::Regex re("a.b");
        auto result = re.search("a b");
        std::cout << "Test 'a.b' on 'a b': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("aXb");
        std::cout << "Test 'a.b' on 'aXb': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("ab");
        std::cout << "Test 'a.b' on 'ab': " << (result ? "Found" : "Not found") << std::endl;
    }

    // Test range repetition
    {
        Regex::Regex re("a{2,4}");
        auto result = re.search("aa");
//        AbstractTree::AST ast("a{2,4}");
//        ast.print();
        std::cout << "Test 'a{2,4}' on 'aa': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("aaa");
        std::cout << "Test 'a{2,4}' on 'aaa': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("aaaa");
        std::cout << "Test 'a{2,4}' on 'aaaa': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("a");
        std::cout << "Test 'a{2,4}' on 'a': " << (result ? "Found" : "Not found") << std::endl;
    }

    // Test escaping
    {
        Regex::Regex re("a&|b");
        auto result = re.search("a|b");
        std::cout << "Test 'a&|b' on 'a|b': " << (result ? "Found" : "Not found") << std::endl;

        result = re.search("ab");
        std::cout << "Test 'a&|b' on 'ab': " << (result ? "Found" : "Not found") << std::endl;
    }
}

void testCaptureGroups() {
    std::cout << "\n=== Testing capture groups ===" << std::endl;

    {
        Regex::Regex re("(<name>a+)b");
        auto result = re.search("aaab");
        if (result) {
            std::cout << "Test '(<name>a+)b' on 'aaab': Found" << std::endl;
        } else {
            std::cout << "Test '(<name>a+)b' on 'aaab': Not found" << std::endl;
        }
    }

    {
        Regex::Regex re("(<first>a+)-(<second>b+)");
        auto result = re.search("aaa-bbb");
        if (result) {
            std::cout << "Test '(<first>a+)-(<second>b+)' on 'aaa-bbb': Found, "
                      << "group 'first' = " << result.groups["first"] << ", "
                      << "group 'second' = " << result.groups["second"] << std::endl;
        } else {
            std::cout << "Test '(<first>a+)-(<second>b+)' on 'aaa-bbb': Not found" << std::endl;
        }
    }
}

void testCompilationAndRegexRecovery() {
    std::cout << "\n=== Testing compilation and regex recovery ===" << std::endl;

    {
        Regex::Regex re("a*b*");
        re.printAutomata();
        std::cout << "Original regex: a*b*" << std::endl;
        std::cout << "Recovered regex 1-path: " << re.toRegex(1) << std::endl;
    }

    {
        Regex::Regex re("(a|b)*abb");
        re.printAutomata();
        std::cout << "\nOriginal regex: (a|b)*abb" << std::endl;
        std::cout << "Recovered regex : " << re.toRegex(1) << std::endl;
    }
    {
        Regex::Regex re("(0+1)*(01)");
        re.printAutomata();
        std::cout << "Original regex: (0+1)*(01)" << std::endl;
        std::cout << "Recovered regex : " << re.toRegex(1) << std::endl;
    }
    {
        Regex::Regex re("(0)*1(0+1)*");
        re.printAutomata();
        std::cout << "Original regex: (0)*1(0+1)*" << std::endl;
        std::cout << "Recovered regex 8-path: " << re.toRegex(1) << std::endl;
    }
    {
        Regex::Regex re(" ");
        re.printAutomata();
        std::cout << "Original regex: " "" << std::endl;
        std::cout << "Recovered regex 1-path: " << re.toRegex(1) << std::endl;
    }

}

void testInverseAndDifference() {
    std::cout << "\n=== Testing inverse and difference operations ===" << std::endl;

    {
        Regex::Regex re("a+");
        Regex::Regex inv = re.inverse();


        auto result = inv.search("b");
        std::cout << "Test inverse of 'a+' on 'b': " << (result ? "Found" : "Not found") << std::endl;

        result = inv.search("a");
        std::cout << "Test inverse of 'a+' on 'a': " << (result ? "Found" : "Not found") << std::endl;
    }

    {
        Regex::Regex re1("a+b+");
        Regex::Regex re2("a+b");
        Regex::Regex diff = re1.difference(re2);

        auto result = diff.search("aabbb");
        std::cout << "Test difference 'a+b+' \\ 'a+b' on 'aabbb': " << (result ? "Found" : "Not found") << std::endl;

        result = diff.search("aab");
        std::cout << "Test difference 'a+b+' \\ 'a+b' on 'aab': " << (result ? "Found" : "Not found") << std::endl;
    }
}

void testSearchFunctionality() {
    std::cout << "\n=== Testing search functionality ===" << std::endl;

    {
        Regex::Regex re("a+b+");
        auto result = re.search("xxaaabbxx");
        if (result) {
            std::cout << "Test 'a+b+' on 'xxaaabbxx': Found '" << result.match << "'" << std::endl;
        } else {
            std::cout << "Test 'a+b+' on 'xxaaabbxx': Not found" << std::endl;
        }
    }

    {
        auto result = Regex::Regex::search("(<word>w....)", "Hello world");
        if (result) {
            std::cout << "Test static search '(w.....)' on 'Hello world': Found '"
                      << result.match << "'" << std::endl;
        } else {
            std::cout << "Test static search '(w....)' on 'Hello world': Not found" << std::endl;
        }
    }
}

int main() {
    try {
        testAutoPrint();
        //testBasicOperations();
        //testCaptureGroups();
       // testCompilationAndRegexRecovery();
    //testInverseAndDifference();
     //testSearchFunctionality();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
//    Prog app;
//    app.run();
//    return 0;
}