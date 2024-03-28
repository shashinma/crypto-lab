//
// Created by Михаил Шашин on 28.03.2024.
//

#include "TempFile.h"

#include <utility>

TempFile::TempFile(std::filesystem::path  path) : path_(std::move(path)) {}

TempFile::~TempFile()
{
    std::filesystem::remove(path_);     // Удаление временного файла
}

const std::filesystem::path& TempFile::path() const { return path_; }
