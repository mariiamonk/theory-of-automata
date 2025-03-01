#include "../statistics/recognizer.h"
#include <regex>
#include <string>
#pragma once

namespace Regex{
    class RegexRecognizer : public Recognizer{
        Statistics::Statistics stat;
    public:
        RegexRecognizer() = default;
        bool validate(const std::string& filename);
    };
}
    