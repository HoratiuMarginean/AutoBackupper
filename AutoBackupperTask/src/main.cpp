#include <conio.h>
#include <iostream>
#include <thread>

#include "archivehandler.h"
#include "settingsfilehandler.h"



int main()
{
	#pragma region ReadSettings
    std::cout << "Reading settings...";

	SettingsFileHandler fileHandler(SETTINGS_FILE);

	std::cout << " done.\n";
	#pragma endregion

	#pragma region PrepareParameters
	std::cout << "Preparing archive parameters...";

	std::wstring destinationPath				  = fileHandler.getDestinationPath();
	std::wstring archiveName					  = fileHandler.getArchiveName();
	bit7z::BitCompressionLevel compressionLevel   = fileHandler.parseCompressionLevel();
	bit7z::UpdateMode updateMode				  = fileHandler.parseUpdateMode();
	const bit7z::BitInOutFormat* archiveFormat	  = fileHandler.parseArchiveFormat();
	bit7z::BitCompressionMethod compressionMethod = fileHandler.parseCompressionMethod();
	uint32_t dictionarySize						  = fileHandler.parseDictionarySize();
	uint32_t threadCount						  = fileHandler.parseThreadCount();
	uint64_t volumeSize							  = fileHandler.parseVolumeSize();
	bool isSolid								  = fileHandler.parseIsSolid();

	std::wstring fullPath = destinationPath + L"\\" + archiveName + L"_" + getDate() + L"_" + getTime();
	if (fileHandler.getArchiveFormat() == L"7z")
		fullPath += L".7z";
	else if (fileHandler.getArchiveFormat() == L"zip")
		fullPath += L".zip";

	std::cout << " done.\n";
	#pragma endregion

	#pragma region Archive
	std::cout << "Archiving...";

	ArchiveHandler archiveHandler(fullPath,
								  updateMode,
								  compressionLevel,
								  *archiveFormat,
								  compressionMethod,
								  dictionarySize,
								  threadCount,
								  volumeSize,
								  isSolid);

	std::thread archivalThread([&] { return archiveHandler.archiveEverything(); });

	while (!archiveHandler.isFinished_)
	{
		std::cout << "\b\b\b   \b\b\b" << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "." << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "." << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "." << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	archivalThread.join();

	std::cout << " done.\n";
	#pragma endregion

    return 0;
}