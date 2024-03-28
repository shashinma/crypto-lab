//
// Created by Михаил Шашин on 28.03.2024.
//

#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <filesystem>
#include <vector>

// Определение константы для расширения файла
constexpr auto EXTENSION = ".enc";

class FileProcessor {
public:
    // Конструктор класса, принимающий пользовательский ключ
    explicit FileProcessor(const std::vector<uint8_t>& customKey);

    // Метод для обработки файла
    std::filesystem::path processFile(const std::filesystem::path& inputFile, bool decrypt);

private:
    // Методы для шифрования и дешифрования файла
    void encryptFile(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv);
    void decryptFile(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv);
    
    // Методы для шифрования и дешифрования файла в памяти
    void encryptFileInMemory(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv);
    void decryptFileInMemory(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv);

    // Ключ для шифрования и дешифрования
    CryptoPP::SecByteBlock key;
};

#endif //FILEPROCESSOR_H
