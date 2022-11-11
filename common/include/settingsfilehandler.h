#pragma once

#include <bitfilecompressor.hpp>

#include "filehandler.h"
#include "utility.h"



class SettingsFileHandler : public FileHandler
{
public:
	SettingsFileHandler(std::wstring settingsFilePath);
	~SettingsFileHandler();

	void saveFile(std::wstring destinationPath,
				  std::wstring archiveName,
				  std::wstring compressionLevel,
				  std::wstring updateMode,
				  std::wstring archiveFormat,
				  std::wstring compressionMethod,
				  std::wstring dictionarySizeAndUnit,
				  std::wstring threadCount,
				  std::wstring volumeSizeAndUnit,
				  std::wstring isSolid);

	std::wstring getDestinationPath();
	std::wstring getArchiveName();
	std::wstring getCompressionLevel();
	std::wstring getUpdateMode();
	std::wstring getArchiveFormat();
	std::wstring getCompressionMethod();
	std::wstring getDictionarySize();
	std::wstring getDictionarySizeUnit();
	std::wstring getThreadCount();
	std::wstring getVolumeSize();
	std::wstring getVolumeSizeUnit();
	std::wstring getIsSolid();

	bit7z::BitCompressionLevel parseCompressionLevel();
	bit7z::UpdateMode parseUpdateMode();
	const bit7z::BitInOutFormat* parseArchiveFormat();
	bit7z::BitCompressionMethod parseCompressionMethod();
	uint32_t parseDictionarySize();
	uint32_t parseThreadCount();
	uint64_t parseVolumeSize();
	bool parseIsSolid();

protected:
	bool isValidDictionarySize();
	bool isValidThreadCount();
	bool isValidVolumeSize();

	void setDefaultDictionarySize();
	void setDefaultThreadCount();

	void writeDefaultFile();

protected:
	std::wstring destinationPath_;
	std::wstring archiveName_;
	std::wstring compressionLevel_;
	std::wstring updateMode_;
	std::wstring archiveFormat_;
	std::wstring compressionMethod_;
	std::wstring dictionarySize_;
	std::wstring dictionarySizeUnit_;
	std::wstring threadCount_;
	std::wstring volumeSize_;
	std::wstring volumeSizeUnit_;
	std::wstring isSolid_;
};