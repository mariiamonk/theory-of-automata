#include "../statistics/recognizer.h"
#include <FlexLexer.h>

#pragma once

namespace Flex{
    class FlexRecognizer : public Recognizer{
        Statistics::Statistics stat;
    public:
        FlexRecognizer() = default;
        bool validate(const std::string& filename);
        void timer();
    };
}
    