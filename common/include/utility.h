#pragma once

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <windows.h>



#ifndef PATH_LIST_FILE
#define PATH_LIST_FILE L"../data/Paths.txt"
#endif

#ifndef SETTINGS_FILE
#define SETTINGS_FILE L"../data/Settings.ini"
#endif

#ifndef SEVEN_ZIP_DLL
#define SEVEN_ZIP_DLL L"../dependencies/7z.dll"
#endif



double computePathSize(std::wstring path);
std::wstring shortenPathSize(double pathSize);

void removeFileLine(std::wstring filePath, std::wstring line);
void appendFileLine(std::wstring filePath, std::wstring line);

bool isDuplicateLine(std::wstring filePath, std::wstring line);
bool isValidFileName(std::wstring fileName);

std::wstring getDate();
std::wstring getTime();

uint32_t getMaxThreadCount();
