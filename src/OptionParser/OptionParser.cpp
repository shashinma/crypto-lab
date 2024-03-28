//
// Created by Михаил Шашин on 28.03.2024.
//

#include <iostream>
#include "OptionsParser.h"

/**
 * Реализация метода parse класса OptionsParser
 * 
 * @typedef cxxopts::ParseResult
 * @return result
 */

cxxopts::ParseResult OptionsParser::parse() const {
    // Создание объекта options для разбора аргументов командной строки
    cxxopts::Options options("untitled6", "Encrypt or decrypt files in a directory");

    // Добавление опций для разбора
    options.add_options()
        ("d,decrypt", "Decrypt files", cxxopts::value<std::string>()->default_value("")) // Установка значения по умолчанию для опции decrypt
        ("h,help", "Print usage"); // Добавление опции help

    // Разбор аргументов командной строки
    auto result = options.parse(argc, argv);

    // Если была указана опция help, выводим справку и завершаем программу
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    return result;
}
