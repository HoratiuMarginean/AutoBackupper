#include "utility.h"



double computePathSize(std::wstring path)
{
	double pathSize = 0;

	std::filesystem::path tempPath(path);

	if (std::filesystem::exists(tempPath))
	{
		if (std::filesystem::is_directory(tempPath))
		{
			std::filesystem::directory_iterator dirIterator(path);
			std::filesystem::directory_iterator endIterator;

			for (dirIterator; dirIterator != endIterator; ++dirIterator)
				pathSize += computePathSize(dirIterator->path());
		}
		else
			pathSize += file_size(tempPath);
	}

	return pathSize;
}
std::wstring shortenPathSize(double pathSize)
{
	short divisionCount = 0;
	while (pathSize >= (1 << 10))
	{
		pathSize /= (1 << 10);

		divisionCount++;
	}

	std::wostringstream sStream;
	sStream << std::fixed << std::setprecision(1);
	sStream << pathSize;
	switch (divisionCount)
	{
		case 0:
			sStream << " B";
			break;
		case 1:
			sStream << " KB";
			break;
		case 2:
			sStream << " MB";
			break;
		case 3:
			sStream << " GB";
			break;
		case 4:
			sStream << " TB";
			break;
		default:
			break;
	}

	return sStream.str();
}

void removeFileLine(std::wstring filePath, std::wstring line)
{
	std::wstring tempPath = L"../data/Temp.txt";

	std::wifstream fileInput(filePath);
	std::wofstream fileOutput(tempPath);

	std::wstring fileLine;
	while (getline(fileInput, fileLine))
		if (fileLine != line)
			fileOutput << fileLine << std::endl;

	fileOutput.close();
	fileInput.close();

	std::filesystem::remove(filePath);
	std::filesystem::rename(tempPath, filePath);
}
void appendFileLine(std::wstring filePath, std::wstring line)
{
	std::wofstream fileOutput(filePath, std::ios_base::app);
	fileOutput << line << std::endl;
	fileOutput.close();
}

bool isDuplicateLine(std::wstring filePath, std::wstring line)
{
	std::wifstream fileInput(filePath);

	std::wstring fileLine;
	while (getline(fileInput, fileLine))
		if (fileLine == line)
			return true;

	return false;
}
bool isValidFileName(std::wstring fileName)
{
	if (fileName.find_first_of(L"\\ / :*? \"<>|") != -1)
		return false;

	std::wstring invalidFileNames[] = { L"CON",  L"PRN",  L"AUX",  L"NUL",
										L"COM1", L"COM2", L"COM3", L"COM4", L"COM5",
										L"COM6", L"COM7", L"COM8", L"COM9",
										L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5",
										L"LPT6", L"LPT7", L"LPT8", L"LPT9" };
	for (std::wstring invalidFileName : invalidFileNames)
		if (fileName.compare(invalidFileName) == 0)
			return false;

	return true;
}

std::wstring getDate()
{
	time_t time = std::time(nullptr);
	tm localTime;
	localtime_s(&localTime, &time);

	std::wostringstream sStream;
	sStream << std::put_time(&localTime, L"%F");

	return sStream.str();
}
std::wstring getTime()
{
	time_t time = std::time(nullptr);
	tm localTime;
	localtime_s(&localTime, &time);

	std::wostringstream sStream;
	sStream << std::put_time(&localTime, L"%H-%M-%S");

	return sStream.str();
}

uint32_t getMaxThreadCount()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	return systemInfo.dwNumberOfProcessors;
}