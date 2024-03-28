//
// Created by Михаил Шашин on 28.03.2024.
//
#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include <cxxopts.hpp>

class OptionsParser {
public:
    // Конструктор класса, принимающий аргументы командной строки
    OptionsParser(const int argc, char* argv[]) : argc(argc), argv(argv) {}

    // Метод для разбора аргументов командной строки
    [[nodiscard]] cxxopts::ParseResult parse() const;

private:
    int argc;
    char** argv;
};

#endif //OPTIONSPARSER_H