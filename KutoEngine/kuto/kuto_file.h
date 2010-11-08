/**
 * @file
 * @brief Fileアクセス
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include <string>
#include <vector>

namespace kuto {

/// ファイルアクセスクラス　（静的クラス）
class File
{
public:
	static bool exists(const char* filename);
	static char* readBytes(const char* filename);
	static char* readBytes(const char* filename, u32& fileSize);
	static void freeBytes(void* pointer);
	static std::string filename(const std::string& filename);
	static std::string filenameWithoutExtension(const std::string& filename);
	static std::string directoryName(const std::string& filename);
	static std::string extension(const std::string& filename);

private:
	File();
	~File();
};	// class File

/// ディレクトリアクセスクラス　（静的クラス）
class Directory
{
public:
	static bool exists(const char* name);
	static bool create(const char* name);
	static std::string homeDirectory();
	static std::vector<std::string> files(const char* name);
	static std::vector<std::string> directories(const char* name);
	static std::vector<std::string> contents(const char* name);

private:
	static std::vector<std::string> contentsImpl(const char* name, bool addFile, bool addDirectory);

	Directory();
	~Directory();
};

}	// namespace kuto
