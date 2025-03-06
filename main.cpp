#include "regex/regex_recognizer.h"
#include "flex/flex_recognizer.h"
#include "statistics/statistics.h"
#include "generator/generator.h"

int main(){
    Flex::FlexRecognizer r;
    StringGenerator g;
    g.generateToFile("input.txt", 10);
    r.validate("input.txt");
}
