cmake_minimum_required(VERSION 3.25.1)
project(theory-of-automata)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests/lib/googletest)
include_directories(tests/lib/googletest/googletest/include)
include_directories(tests/lib/googletest/googlemock/include)

add_executable(theory-of-automata main.cpp
        AST/AST.cpp
        AST/AST.h
        AST/ASTNode.h
        AST/CharNode.cpp
        AST/CharNode.h
        AST/Operations.cpp
        AST/Operations.h
        AST/StackNode.cpp
        AST/StackNode.h
        NFA/NFA.cpp
        NFA/NFA.h
        DFA/DFA.cpp
        DFA/DFA.h
        DFA/MinimizedDFA.cpp
        DFA/MinimizedDFA.h
        Regex/Regex.cpp
        Regex/Regex.h
        tests/tests_a/test.cpp
        tests/tests_a/test_reg.cpp
        tests/tests_a/test_nfa.cpp
)

target_link_libraries(theory-of-automata gtest gtest_main)