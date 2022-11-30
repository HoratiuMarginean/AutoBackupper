#pragma once

#include <bitfilecompressor.hpp>

#include <atomic>
#include <vector>

#include "utility.h"



class ArchiveHandler
{
public:
	ArchiveHandler(std::wstring destinationPath,
				   bit7z::UpdateMode updateMode,
				   bit7z::BitCompressionLevel level,
				   const bit7z::BitInOutFormat& format,
				   bit7z::BitCompressionMethod method,
				   uint32_t dictionarySize,
				   uint32_t threadCount,
				   uint64_t volumeSize,
				   bool isSolid);
	~ArchiveHandler();

	void initArchivePath(std::wstring path);
	void initArchivablePaths();

	void archiveEverything();

public:
	std::atomic<bool> isFinished_;
	std::atomic<bool> isCancelled_;

	std::atomic<uint64_t> archivalProgress_;

protected:
	std::shared_ptr<bit7z::Bit7zLibrary> lib_;
	std::shared_ptr<bit7z::BitFileCompressor> compressor_;

	std::wstring archivePath_;
	std::vector<std::wstring> archivablePaths_;

	uint64_t totalSize_;
};
