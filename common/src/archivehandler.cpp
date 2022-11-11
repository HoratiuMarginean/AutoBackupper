#include "archivehandler.h"

using namespace bit7z;

ArchiveHandler::ArchiveHandler(std::wstring destinationPath,
							   bit7z::UpdateMode updateMode,
							   bit7z::BitCompressionLevel level,
							   const bit7z::BitInOutFormat& format,
							   bit7z::BitCompressionMethod method,
							   uint32_t dictionarySize,
							   uint32_t threadCount,
							   uint64_t volumeSize,
							   bool isSolid)
{
	isFinished_ = false;
	isCancelled_ = false;

	archivalProgress_ = 0;

	lib_ = std::make_shared<Bit7zLibrary>(SEVEN_ZIP_DLL);
	compressor_ = std::make_shared<BitFileCompressor>(*lib_, format);

	initArchivePath(destinationPath);
	initArchivablePaths();

	compressor_->setCompressionLevel(level);
	compressor_->setUpdateMode(updateMode);
	compressor_->setCompressionMethod(method);
	compressor_->setDictionarySize(dictionarySize);
	compressor_->setThreadsCount(threadCount);
	compressor_->setVolumeSize(volumeSize);
	compressor_->setSolidMode(isSolid);

	compressor_->setTotalCallback(
		[this](uint64_t total_size)
		{
			totalSize_ = total_size;
		}
	);
	compressor_->setProgressCallback(
		[this](uint64_t progress_size)
		{
			archivalProgress_ = static_cast<int>((100.0 * progress_size) / totalSize_);

			return !isCancelled_;
		}
	);
}
ArchiveHandler::~ArchiveHandler() {}

void ArchiveHandler::initArchivePath(std::wstring path)
{
	archivePath_ = path;
}
void ArchiveHandler::initArchivablePaths()
{
	std::wifstream fileInput(PATH_LIST_FILE);
	std::wstring archivablePath;

	while (getline(fileInput, archivablePath))
		archivablePaths_.push_back(archivablePath);
}

void ArchiveHandler::archiveEverything()
{
	try
	{
		compressor_->compress(archivablePaths_, archivePath_);
	}
	catch (const std::exception& e)
	{
		OutputDebugStringA(e.what());
	}

	isFinished_ = true;
}