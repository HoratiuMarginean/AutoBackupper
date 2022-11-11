#pragma once

#include <filesystem>
#include <string>



class FileHandler
{
public:
	FileHandler(std::wstring filePath);
	~FileHandler();

protected:
	std::filesystem::path filePath_;
};