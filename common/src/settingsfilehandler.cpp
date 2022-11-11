#include "settingsfilehandler.h"

SettingsFileHandler::SettingsFileHandler(std::wstring settingsFilePath) : FileHandler(settingsFilePath)
{
	bool needsRewrite = false;

	destinationPath_	= L"";
	archiveName_		= L"Backup";
	compressionLevel_	= L"Normal";
	updateMode_			= L"Overwrite";
	archiveFormat_		= L"7z";
	compressionMethod_	= L"LZMA2";
	dictionarySize_		= L"16";
	dictionarySizeUnit_ = L"MB";
	threadCount_		= std::to_wstring(getMaxThreadCount());
	volumeSize_			= L"0";
	volumeSizeUnit_		= L"MB";
	isSolid_			= L"False";

	if (std::filesystem::exists(filePath_))
	{
		std::wifstream fileInput(filePath_);

		std::wstring line;

		getline(fileInput, line);
		if (line == L"[Archive]")
		{
			std::vector<std::wstring> lines;
			while (getline(fileInput, line))
				lines.push_back(line);

			bool found;
			std::wstring propertyName;
			std::wstring sep;
			std::wstring value;

			#pragma region DestinationPath
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"DestinationPath") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					destinationPath_ = value;

					found = true;

					break;
				}
			}
			if (!found)
				needsRewrite = true;
			#pragma endregion

			#pragma region ArchiveName
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"ArchiveName") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					archiveName_ = value;

					found = true;

					break;
				}
			}
			if (!found)
				needsRewrite = true;

			#pragma endregion

			#pragma region CompressionLevel
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"CompressionLevel") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);

					if (value == L"copy")
						compressionLevel_ = L"Copy";
					else if (value == L"fastest")
						compressionLevel_ = L"Fastest";
					else if (value == L"fast")
						compressionLevel_ = L"Fast";
					else if (value == L"normal")
						compressionLevel_ = L"Normal";
					else if (value == L"high")
						compressionLevel_ = L"High";
					else if (value == L"max")
						compressionLevel_ = L"Max";

					found = true;

					break;
				}
			}
			if (!found)
				needsRewrite = true;
			#pragma endregion

			#pragma region UpdateMode
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"UpdateMode") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);

					if (value == L"none")
						updateMode_ = L"None";
					else if (value == L"append")
						updateMode_ = L"Append";
					else if (value == L"overwrite")
						updateMode_ = L"Overwrite";

					found = true;

					break;
				}
			}
			if (!found)
				needsRewrite = true;
			#pragma endregion

			#pragma region ArchiveFormat
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"ArchiveFormat") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);

					if (value == L"7z")
						archiveFormat_ = L"7z";
					else if (value == L"zip")
						archiveFormat_ = L"zip";

					found = true;

					break;
				}
			}
			if (!found)
				needsRewrite = true;
			#pragma endregion

			#pragma region CompressionMethod
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"CompressionMethod") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);

					if (archiveFormat_ == L"7z")
					{
						if (value == L"bzip2")
							compressionMethod_ = L"BZip2";
						else if (value == L"lzma")
							compressionMethod_ = L"LZMA";
						else if (value == L"lzma2")
							compressionMethod_ = L"LZMA2";
						else if (value == L"ppmd")
							compressionMethod_ = L"PPMd";
					}
					else if (archiveFormat_ == L"zip")
					{
						if (value == L"bzip2")
							compressionMethod_ = L"BZip2";
						else if (value == L"lzma")
							compressionMethod_ = L"LZMA";
						else if (value == L"ppmd")
							compressionMethod_ = L"PPMd";
						else if (value == L"deflate")
							compressionMethod_ = L"Deflate";
						else if (value == L"deflate64")
							compressionMethod_ = L"Deflate64";
					}

					found = true;

					break;
				}
			}
			if (!found)
			{
				needsRewrite = true;

				if (archiveFormat_ == L"7z")
					compressionMethod_ = L"LZMA2";
				else if (archiveFormat_ == L"zip")
					compressionMethod_ = L"Deflate";
			}
			#pragma endregion

			#pragma region DictionarySize
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"DictionarySize") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					std::wstring unit;
					sStream >> propertyName >> sep >> value >> unit;
					std::transform(unit.begin(), unit.end(), unit.begin(), ::toupper);

					dictionarySize_ = value;
					dictionarySizeUnit_ = unit;

					if (!isValidDictionarySize())
						setDefaultDictionarySize();

					found = true;

					break;
				}
			}
			if (!found)
			{
				needsRewrite = true;

				setDefaultDictionarySize();
			}
			#pragma endregion

			#pragma region ThreadCount
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"ThreadCount") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					threadCount_ = value;

					if (!isValidThreadCount())
						setDefaultThreadCount();

					found = true;

					break;
				}
			}
			if (!found)
			{
				needsRewrite = true;

				setDefaultThreadCount();
			}
			#pragma endregion

			#pragma region VolumeSize
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"VolumeSize") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					std::wstring unit;
					sStream >> propertyName >> sep >> value >> unit;
					std::transform(unit.begin(), unit.end(), unit.begin(), ::toupper);

					volumeSize_ = value;
					volumeSizeUnit_ = unit;

					if (!isValidVolumeSize())
					{
						volumeSize_ = L"0";
						volumeSizeUnit_ = L"MB";
					}

					found = true;

					break;
				}
			}
			if (!found)
				needsRewrite = true;
			#pragma endregion

			#pragma region SolidArchive
			found = false;
			for (int i = 0; i < lines.size(); i++)
			{
				if (lines[i].find(L"SolidArchive") != std::wstring::npos)
				{
					std::wstringstream sStream(lines[i]);
					lines.erase(lines.begin() + i);

					sStream >> propertyName >> sep >> value;
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);

					if (value == L"false")
						isSolid_ = L"False";
					else if (value == L"true")
						isSolid_ = L"True";

					found = true;

					break;
				}
			}
			if (!found)
				needsRewrite = true;
			#pragma endregion	
		}
		else
			needsRewrite = true;
	}
	else
		needsRewrite = true;

	if (needsRewrite)
		writeDefaultFile();
}
SettingsFileHandler::~SettingsFileHandler() {}

void SettingsFileHandler::saveFile(std::wstring destinationPath,
								   std::wstring archiveName,
								   std::wstring compressionLevel,
								   std::wstring updateMode,
								   std::wstring archiveFormat,
								   std::wstring compressionMethod,
								   std::wstring dictionarySizeAndUnit,
								   std::wstring threadCount,
								   std::wstring volumeSizeAndUnit,
								   std::wstring isSolid)
{
	std::wofstream fileOutput(filePath_);

	// Header
	fileOutput << L"[Archive]" << std::endl;

	// Destination Path
	fileOutput << L"DestinationPath = " << destinationPath << std::endl;

	// Archive Name
	fileOutput << L"ArchiveName = " << archiveName << std::endl;

	// Compression Level
	fileOutput << L"CompressionLevel = " << compressionLevel << std::endl;

	// Update Mode
	fileOutput << L"UpdateMode = " << updateMode << std::endl;

	// Archive Format
	fileOutput << L"ArchiveFormat = " << archiveFormat << std::endl;

	// Compression Method
	fileOutput << L"CompressionMethod = " << compressionMethod << std::endl;

	// Dictionary Size
	fileOutput << L"DictionarySize = " << dictionarySizeAndUnit << std::endl;

	// Thread Count
	fileOutput << L"ThreadCount = " << threadCount << std::endl;

	// Volume Size
	fileOutput << L"VolumeSize = " << volumeSizeAndUnit << std::endl;

	// Solid Archive
	fileOutput << L"SolidArchive = " << isSolid;
}

std::wstring SettingsFileHandler::getDestinationPath()
{
	return destinationPath_;
}
std::wstring SettingsFileHandler::getArchiveName()
{
	return archiveName_;
}
std::wstring SettingsFileHandler::getCompressionLevel()
{
	return compressionLevel_;
}
std::wstring SettingsFileHandler::getUpdateMode()
{
	return updateMode_;
}
std::wstring SettingsFileHandler::getArchiveFormat()
{
	return archiveFormat_;
}
std::wstring SettingsFileHandler::getCompressionMethod()
{
	return compressionMethod_;
}
std::wstring SettingsFileHandler::getDictionarySize()
{
	return dictionarySize_;
}
std::wstring SettingsFileHandler::getDictionarySizeUnit()
{
	return dictionarySizeUnit_;
}
std::wstring SettingsFileHandler::getThreadCount()
{
	return threadCount_;
}
std::wstring SettingsFileHandler::getVolumeSize()
{
	return volumeSize_;
}
std::wstring SettingsFileHandler::getVolumeSizeUnit()
{
	return volumeSizeUnit_;
}
std::wstring SettingsFileHandler::getIsSolid()
{
	return isSolid_;
}

bit7z::BitCompressionLevel SettingsFileHandler::parseCompressionLevel()
{
	if (compressionLevel_ == L"Copy")
		return bit7z::BitCompressionLevel::None;
	else if (compressionLevel_ == L"Fastest")
		return bit7z::BitCompressionLevel::Fastest;
	else if (compressionLevel_ == L"Fast")
		return bit7z::BitCompressionLevel::Fast;
	else if (compressionLevel_ == L"Normal")
		return bit7z::BitCompressionLevel::Normal;
	else if (compressionLevel_ == L"High")
		return bit7z::BitCompressionLevel::Max;
	else if (compressionLevel_ == L"Max")
		return bit7z::BitCompressionLevel::Ultra;
	else // It is not possible to get here
		return bit7z::BitCompressionLevel::Normal;
}
bit7z::UpdateMode SettingsFileHandler::parseUpdateMode()
{
	if (updateMode_ == L"None")
		return bit7z::UpdateMode::None;
	else if (updateMode_ == L"Append")
		return bit7z::UpdateMode::Append;
	else if (updateMode_ == L"Overwrite")
		return bit7z::UpdateMode::Overwrite;
	else // It is not possible to get here
		return bit7z::UpdateMode::Overwrite;
}
const bit7z::BitInOutFormat* SettingsFileHandler::parseArchiveFormat()
{
	if (archiveFormat_ == L"7z")
		return &bit7z::BitFormat::SevenZip;
	else if (archiveFormat_ == L"zip")
		return &bit7z::BitFormat::Zip;
	else // It is not possible to get here
		return &bit7z::BitFormat::SevenZip;
}
bit7z::BitCompressionMethod SettingsFileHandler::parseCompressionMethod()
{
	if (compressionMethod_ == L"BZip2")
		return bit7z::BitCompressionMethod::BZip2;
	else if (compressionMethod_ == L"LZMA")
		return bit7z::BitCompressionMethod::Lzma;
	else if (compressionMethod_ == L"LZMA2")
		return bit7z::BitCompressionMethod::Lzma2;
	else if (compressionMethod_ == L"PPMd")
		return bit7z::BitCompressionMethod::Ppmd;
	else if (compressionMethod_ == L"Deflate")
		return bit7z::BitCompressionMethod::Deflate;
	else if (compressionMethod_ == L"Deflate64")
		return bit7z::BitCompressionMethod::Deflate64;
	else // It is not possible to get here
		return bit7z::BitCompressionMethod::Lzma;
}
uint32_t SettingsFileHandler::parseDictionarySize()
{
	uint32_t dictionarySize = stoi(dictionarySize_);

	if (dictionarySizeUnit_ == L"KB")
		dictionarySize *= (1 << 10);
	else if(dictionarySizeUnit_ == L"MB")
		dictionarySize *= (1 << 20);

	return dictionarySize;
}
uint32_t SettingsFileHandler::parseThreadCount()
{
	return stoi(threadCount_);
}
uint64_t SettingsFileHandler::parseVolumeSize()
{
	uint64_t volumeSize = stoi(volumeSize_);

	if(volumeSizeUnit_ == L"B")
		volumeSize *= (1 << 0);
	else if (volumeSizeUnit_ == L"KB")
		volumeSize *= (1 << 10);
	else if (volumeSizeUnit_ == L"MB")
		volumeSize *= (1 << 20);
	else if (volumeSizeUnit_ == L"GB")
		volumeSize *= (1 << 30);

	return volumeSize;
}
bool SettingsFileHandler::parseIsSolid()
{
	if (isSolid_ == L"True")
		return true;
	else
		return false;
}

bool SettingsFileHandler::isValidDictionarySize()
{
	std::wstring dictionarySizeAndUnit = dictionarySize_ + L" " + dictionarySizeUnit_;

	if (archiveFormat_ == L"7z")
	{
		if (compressionMethod_ == L"BZip2")
		{
			const std::wstring dictionarySizes[] = { L"100 KB", L"200 KB", L"300 KB",
													 L"400 KB", L"500 KB", L"600 KB",
													 L"700 KB", L"800 KB", L"900 KB" };

			for (std::wstring dictionarySize : dictionarySizes)
				if (dictionarySizeAndUnit == dictionarySize)
					return true;

			return false;
		}
		else if (compressionMethod_ == L"LZMA")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			for (std::wstring dictionarySize : dictionarySizes)
				if (dictionarySizeAndUnit == dictionarySize)
					return true;

			return false;
		}
		else if (compressionMethod_ == L"LZMA2")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			for (std::wstring dictionarySize : dictionarySizes)
				if (dictionarySizeAndUnit == dictionarySize)
					return true;

			return false;
		}
		else if (compressionMethod_ == L"PPMd")
		{
			const std::wstring dictionarySizes[] = { L"1 MB",   L"2 MB",   L"4 MB",
													 L"8 MB",   L"16 MB",  L"32 MB",
													 L"64 MB",  L"128 MB", L"256 MB",
													 L"512 MB", L"1024 MB" };

			for (std::wstring dictionarySize : dictionarySizes)
				if (dictionarySizeAndUnit == dictionarySize)
					return true;

			return false;
		}
	}
	else if (archiveFormat_ == L"zip")
	{
		if (compressionMethod_ == L"BZip2")
		{
			const std::wstring dictionarySizes[] = { L"100 KB", L"200 KB", L"300 KB",
													 L"400 KB", L"500 KB", L"600 KB",
													 L"700 KB", L"800 KB", L"900 KB" };

			for (std::wstring dictionarySize : dictionarySizes)
				if (dictionarySizeAndUnit == dictionarySize)
					return true;

			return false;
		}
		else if (compressionMethod_ == L"LZMA")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			for (std::wstring dictionarySize : dictionarySizes)
				if (dictionarySizeAndUnit == dictionarySize)
					return true;

			return false;
		}
		else if (compressionMethod_ == L"PPMd")
		{
			const std::wstring dictionarySizes[] = { L"1 MB",   L"2 MB",   L"4 MB",
													 L"8 MB",   L"16 MB",  L"32 MB",
													 L"64 MB",  L"128 MB", L"256 MB" };

			for (std::wstring dictionarySize : dictionarySizes)
				if (dictionarySizeAndUnit == dictionarySize)
					return true;

			return false;
		}
		else if (compressionMethod_ == L"Deflate")
		{
			const std::wstring dictionarySize = { L"32 KB" };

			if (dictionarySizeAndUnit == dictionarySize)
				return true;

			return false;
		}
		else if (compressionMethod_ == L"Deflate64")
		{
			const std::wstring dictionarySize = { L"64 KB" };

			if (dictionarySizeAndUnit == dictionarySize)
				return true;

			return false;
		}
	}

	return false;
}
bool SettingsFileHandler::isValidThreadCount()
{
	uint32_t maxThreadCount = getMaxThreadCount();
	uint32_t threadCount = stoi(threadCount_);

	if (archiveFormat_ == L"7z")
	{
		if (compressionMethod_ == L"BZip2" ||
			compressionMethod_ == L"LZMA2")
		{
			if (threadCount >= 1 && threadCount <= maxThreadCount)
				return true;

			return false;
		}
		else if (compressionMethod_ == L"LZMA")
		{
			if (threadCount >= 1 && threadCount <= 2 && threadCount <= maxThreadCount)
				return true;

			return false;
		}
		else if (compressionMethod_ == L"PPMd")
		{
			if (threadCount == 1)
				return true;

			return false;
		}
	}
	else if (archiveFormat_ == L"zip")
	{
		if (compressionMethod_ == L"BZip2" ||
			compressionMethod_ == L"LZMA" ||
			compressionMethod_ == L"PPMd" ||
			compressionMethod_ == L"Deflate" ||
			compressionMethod_ == L"Deflate64")
		{
			if (threadCount >= 1 && threadCount <= maxThreadCount)
				return true;

			return false;
		}
	}

	return false;
}
bool SettingsFileHandler::isValidVolumeSize()
{
	if (stoi(volumeSize_) >= 0)
		return true;

	if (volumeSizeUnit_ == L"B" ||
		volumeSizeUnit_ == L"KB" ||
		volumeSizeUnit_ == L"MB" ||
		volumeSizeUnit_ == L"GB")
		return true;

	return false;
}

void SettingsFileHandler::setDefaultDictionarySize()
{
	if (archiveFormat_ == L"7z")
	{
		if (compressionMethod_ == L"BZip2")
		{
			dictionarySize_ = L"900";
			dictionarySizeUnit_ = L"KB";
		}
		else if (compressionMethod_ == L"LZMA" ||
				 compressionMethod_ == L"LZMA2" ||
				 compressionMethod_ == L"PPMd")
		{
			dictionarySize_ = L"16";
			dictionarySizeUnit_ = L"MB";
		}
	}
	else if (archiveFormat_ == L"zip")
	{
		if (compressionMethod_ == L"BZip2")
		{
			dictionarySize_ = L"900";
			dictionarySizeUnit_ = L"KB";
		}
		else if (compressionMethod_ == L"LZMA" ||
				 compressionMethod_ == L"PPMd")
		{
			dictionarySize_ = L"16";
			dictionarySizeUnit_ = L"MB";
		}
		else if (compressionMethod_ == L"Deflate")
		{
			dictionarySize_ = L"32";
			dictionarySizeUnit_ = L"KB";
		}
		else if (compressionMethod_ == L"Deflate64")
		{
			dictionarySize_ = L"64";
			dictionarySizeUnit_ = L"KB";
		}	
	}
}
void SettingsFileHandler::setDefaultThreadCount()
{
	uint32_t maxThreadCount = getMaxThreadCount();
	uint32_t threadCount = maxThreadCount;

	if (archiveFormat_ == L"7z")
	{
		if (compressionMethod_ == L"BZip2" ||
			compressionMethod_ == L"LZMA2")
			threadCount = maxThreadCount;
		else if (compressionMethod_ == L"LZMA")
			if (maxThreadCount >= 2)
				threadCount = 2;
			else
				threadCount = maxThreadCount;
		else if (compressionMethod_ == L"PPMd")
			threadCount = 1;
	}
	else if (archiveFormat_ == L"zip")
	{
		if (compressionMethod_ == L"BZip2" ||
			compressionMethod_ == L"LZMA" ||
			compressionMethod_ == L"PPMd" ||
			compressionMethod_ == L"Deflate" ||
			compressionMethod_ == L"Deflate64")
			threadCount = maxThreadCount;
	}

	threadCount_ = threadCount;
}

void SettingsFileHandler::writeDefaultFile()
{
	std::wofstream fileOutput(filePath_);

	// Header
	fileOutput << L"[Archive]" << std::endl;

	// Destination Path
	fileOutput << L"DestinationPath = " << destinationPath_ << std::endl;

	// Archive Name
	fileOutput << L"ArchiveName = " << archiveName_ << std::endl;

	// Compression Level
	fileOutput << L"CompressionLevel = " << compressionLevel_ << std::endl;

	// Update Mode
	fileOutput << L"UpdateMode = " << updateMode_ << std::endl;

	// Archive Format
	fileOutput << L"ArchiveFormat = " << archiveFormat_ << std::endl;

	// Compression Method
	fileOutput << L"CompressionMethod = " << compressionMethod_ << std::endl;

	// Dictionary Size
	fileOutput << L"DictionarySize = " << dictionarySize_ << L" " << dictionarySizeUnit_ << std::endl;

	// Thread Count
	fileOutput << L"ThreadCount = " << threadCount_ << std::endl;

	// Volume Size
	fileOutput << L"VolumeSize = " << volumeSize_ << L" " << volumeSizeUnit_ << std::endl;

	// Solid Archive
	fileOutput << L"SolidArchive = " << isSolid_;
}