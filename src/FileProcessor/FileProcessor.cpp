//
// Created by Михаил Шашин on 28.03.2024.
//

#include "FileProcessor.h"
#include "TempFile/TempFile.h"

FileProcessor::FileProcessor(const std::vector<uint8_t>& customKey) {
    key = CryptoPP::SecByteBlock(customKey.data(), customKey.size());   // Инициализация ключа
}


/**
 * Реализация метода  обработки файла
 * 
 * @param inputFile 
 * @param decrypt 
 * @return std::filesystem::path
 */
std::filesystem::path FileProcessor::processFile(const std::filesystem::path& inputFile, bool decrypt) {
    std::filesystem::path outputFile = inputFile;

    // Удаление или добавление расширения в зависимости от режима
    if (decrypt) {
        outputFile.replace_extension("");   // Удаление расширения ".enc"
    } else {
        outputFile += EXTENSION;    // Добавление расширения ".enc"
    }

    // Инициализация вектора для вектора инициализации (IV)
    std::vector<CryptoPP::byte> iv(CryptoPP::AES::BLOCKSIZE);

    // Подсчет количества файлов в директории
    size_t fileCount = std::distance(std::filesystem::directory_iterator(inputFile.parent_path()), std::filesystem::directory_iterator{});

    // Выбор метода шифрования в зависимости от количества файлов
    if (fileCount > 5) {
        if (decrypt) {
            decryptFile(inputFile, outputFile, iv);
        } else {
            encryptFile(inputFile, outputFile, iv);
        }
    } else {
        if (decrypt) {
            decryptFileInMemory(inputFile, outputFile, iv);
        } else {
            encryptFileInMemory(inputFile, outputFile, iv);
        }
    }

    // Удаление исходного файла после шифрования/дешифрования
    std::filesystem::remove(inputFile);

    return outputFile;
}

/**
 * Реализация метода  шифрования файла
 * 
 * @param inputFile 
 * @param outputFile 
 * @param iv
 */
void FileProcessor::encryptFile(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv)
{
    // Генерация случайного вектора инициализации
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock(iv.data(), CryptoPP::AES::BLOCKSIZE);

    // Шифрование файла
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(key, key.size(), iv.data());
    CryptoPP::FileSource fileSource(inputFile.c_str(), true, new CryptoPP::StreamTransformationFilter(encryptor, new CryptoPP::FileSink(outputFile.c_str(), true)));

    // Добавление IV в конец файла
    std::ofstream out(outputFile.c_str(), std::ios::app | std::ios::binary);
    out.write(reinterpret_cast<const char*>(iv.data()), iv.size());
}

/**
 * Реалищаия метода дешифрования файла
 * 
 * @param inputFile 
 * @param outputFile 
 * @param iv 
 */
void FileProcessor::decryptFile(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv)
{
    // Чтение IV из конца файла
    std::ifstream in(inputFile.c_str(), std::ios::binary);
    in.seekg(-CryptoPP::AES::BLOCKSIZE, std::ios::end);
    in.read(reinterpret_cast<char*>(iv.data()), iv.size());

    // Создание временного файла и копирование в него зашифрованного файла
    TempFile tempFile(inputFile.string() + ".tmp");
    std::filesystem::copy(inputFile, tempFile.path());

    // Удаление IV из временного файла
    std::uintmax_t fileSize = std::filesystem::file_size(tempFile.path());
    std::filesystem::resize_file(tempFile.path(), fileSize - CryptoPP::AES::BLOCKSIZE);

    // Дешифрование файла
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(key, key.size(), iv.data());
    CryptoPP::FileSource(tempFile.path().c_str(), true, new CryptoPP::StreamTransformationFilter(decryptor, new CryptoPP::FileSink(outputFile.c_str())));
}

/**
 * Метод для шифрования файла в памяти
 * 
 * @param inputFile 
 * @param outputFile 
 * @param iv 
 */
void FileProcessor::encryptFileInMemory(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv)
{
    // Генерация случайного вектора инициализации
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock(iv.data(), CryptoPP::AES::BLOCKSIZE);

    // Чтение всего файла в память
    std::ifstream in(inputFile.c_str(), std::ios::binary | std::ios::ate);
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!in.read(buffer.data(), size))
    {
        throw std::runtime_error("Failed to read file");
    }

    // Шифрование буфера в памяти
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor(key, key.size(), iv.data());
    CryptoPP::ArraySource arraySource(reinterpret_cast<CryptoPP::byte*>(buffer.data()), buffer.size(), true, new CryptoPP::StreamTransformationFilter(encryptor, new CryptoPP::ArraySink(reinterpret_cast<CryptoPP::byte*>(buffer.data()), buffer.size())));

    // Добавление IV в конец буфера
    buffer.insert(buffer.end(), iv.begin(), iv.end());

    // Запись зашифрованного буфера обратно в файл
    std::ofstream out(outputFile.c_str(), std::ios::binary);
    out.write(buffer.data(), buffer.size());
}

/**
 * Метод для дешифрования файла в памяти
 * @param inputFile 
 * @param outputFile 
 * @param iv 
 */
void FileProcessor::decryptFileInMemory(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile, std::vector<CryptoPP::byte>& iv)
{
    // Чтение всего файла в память
    std::ifstream in(inputFile.c_str(), std::ios::binary | std::ios::ate);
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!in.read(buffer.data(), size))
    {
        throw std::runtime_error("Failed to read file");
    }

    // Извлечение IV из конца буфера
    std::copy(buffer.end() - CryptoPP::AES::BLOCKSIZE, buffer.end(), iv.begin());

    // Удаление IV из буфера
    buffer.resize(buffer.size() - CryptoPP::AES::BLOCKSIZE);

    // Дешифрование буфера в памяти
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryptor(key, key.size(), iv.data());
    CryptoPP::ArraySource arraySource(reinterpret_cast<CryptoPP::byte*>(buffer.data()), buffer.size(), true, new CryptoPP::StreamTransformationFilter(decryptor, new CryptoPP::ArraySink(reinterpret_cast<CryptoPP::byte*>(buffer.data()), buffer.size())));

    // Запись расшифрованного буфера обратно в файл
    std::ofstream out(outputFile.c_str(), std::ios::binary);
    out.write(buffer.data(), buffer.size());
}