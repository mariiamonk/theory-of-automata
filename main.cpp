#include "regex/regex_recognizer.h"
#include "flex/flex_recognizer.h"
#include "statistics/statistics.h"
#include "generator/generator.h"

int main(){
    Flex::FlexRecognizer r;
    Generator::StringGenerator g;
    //r.timer();
    g.generateToFile("input.txt", 10, 0);
    r.validate("input.txt");
}
