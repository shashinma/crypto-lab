//
// Created by Михаил Шашин on 28.03.2024.
//

#ifndef TEMPFILE_H
#define TEMPFILE_H

#include <filesystem>


/**
 *  Класс для создания временного файла
 */
class TempFile
{
public:
    explicit TempFile(std::filesystem::path  path);    // Конструктор класса TempFile, принимающий путь к временному файлу
    ~TempFile();    // Деструктор класса TempFile, удаляющий временный файл

    [[nodiscard]] const std::filesystem::path& path() const;

private:
    std::filesystem::path path_;    // Метод для получения пути к временному файлу
};

#endif //TEMPFILE_H