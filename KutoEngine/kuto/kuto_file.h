/**
 * @file
 * @brief Fileアクセス
 * @author takuto
 */
#pragma once

#include "kuto_types.h"
#include <string>

namespace kuto {

/// ファイルアクセスクラス　（静的クラス）
class File
{
public:
	static bool exists(const char* filename);
	static char* readBytes(const char* filename);
	static char* readBytes(const char* filename, u32& fileSize);
	static void freeBytes(void* pointer);
	static std::string getFileName(const std::string& filename);
	static std::string getFileNameWithoutExtension(const std::string& filename);
	static std::string getDirectoryName(const std::string& filename);
	static std::string getExtension(const std::string& filename);

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
	static std::string getHomeDirectory();

private:
	Directory();
	~Directory();
};

}	// namespace kuto
