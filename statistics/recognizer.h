#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include <vector>
#include "statistics.h"
#include "../generator/generator.h"

#pragma once

class Recognizer{
public:
    virtual ~Recognizer() = default;
    virtual bool validate(const std::string& input) = 0;
    virtual void timer() = 0;
};

