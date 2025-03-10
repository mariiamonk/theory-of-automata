#include "regex/regex_recognizer.h"
#include "flex/flex_recognizer.h"
#include "statistics/statistics.h"
#include "generator/generator.h"
#include <chrono>
#include <iostream>
#include <memory>

#pragma once

namespace prog{
    std::unique_ptr<Recognizer> createRecognizer(const std::string& type);
    void runTimerMode(const std::string& recognizerType, const std::string& filePath);
    void runNormalMode(const std::string& recognizerType, const std::string& filePath, int numStrings);
}