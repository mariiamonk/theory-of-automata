#include <iostream>
#include "statistics.h"

#pragma once

class Recognizer{
public:
    virtual ~Recognizer() = default;
    virtual bool validate(const std::string& input) = 0;
};

