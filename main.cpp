#include "regex/regex_recognizer.h"
#include "statistics/statistics.h"
#include "generator/generator.h"

int main(){
    Regex::RegexRecognizer r;
    StringGenerator g;
    g.generateToFile("input.txt", 100);
    r.validate("input.txt");
}
