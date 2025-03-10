cmake_minimum_required(VERSION 3.10.0)
project(recognizer VERSION 0.1.0 LANGUAGES C CXX)

add_executable(recognizer main.cpp statistics/statistics.h statistics/statistics.cpp statistics/recognizer.h 
              regex/regex_recognizer.cpp regex/regex_recognizer.h 
              generator/generator.h generator/generator.cpp 
              flex/flex_recognizer.h flex/flex_recognizer.cpp flex/lex.yy.cc 
              smc/AppClass.cpp smc/AppClass.h smc/AppClass.sm
              functions.h functions.cpp)   