cmake_minimum_required(VERSION 3.30)
project(theory-of-automata)

set(CMAKE_CXX_STANDARD 20)

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
        Menu/Prog.cpp
        Menu/Prog.h
)