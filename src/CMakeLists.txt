cmake_minimum_required(VERSION 3.28)
project(ransomware)

set(CMAKE_CXX_STANDARD 23)

# Имена библиотек и исполняемых файлов
set(LIBRARY_FILEPROC fileprocessor)
set(LIBRARY_OPTPARS optionparser)
set(LIBRARY_TEMPFILE tempfile)
set(EXECUTABLE_NAME ransomware)

find_package(cryptopp CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

add_library(${LIBRARY_FILEPROC} FileProcessor.cpp FileProcessor.h)
add_library(${LIBRARY_OPTPARS} OptionParser.cpp OptionsParser.h)
add_library(${LIBRARY_TEMPFILE} TempFile.cpp TempFile.h)

target_link_libraries(${LIBRARY_FILEPROC} PRIVATE cryptopp::cryptopp)
target_link_libraries(${LIBRARY_FILEPROC} INTERFACE cxxopts::cxxopts)
target_link_libraries(${LIBRARY_OPTPARS} PRIVATE cxxopts::cxxopts)

# Указание путей к заголовочным файлам
target_include_directories(${LIBRARY_FILEPROC} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${LIBRARY_OPTPARS} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_FILEPROC} ${LIBRARY_TEMPFILE} ${LIBRARY_OPTPARS})