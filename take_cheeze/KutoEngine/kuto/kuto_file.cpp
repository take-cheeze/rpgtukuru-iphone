#include "kuto_file.h"
#include "kuto_error.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

using namespace std;

namespace kuto {

/**
 * ファイルが存在します？
 * @param filename		ファイル名
 * @retval true			存在するよ
 * @retval false		ありません
 */
bool File::exists(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if(fp != NULL) {
		fclose(fp);
		return true;
	} else return false;
}

/**
 * ファイル読み込み
 * @param filename		ファイル名
 * @return				ファイルのバイト列（freeBytesで解放してね）
 */
char* File::readBytes(const char* filename)
{
	u32 tempFileSize;
	return readBytes(filename, tempFileSize);
}

/**
 * ファイル読み込み
 * @param filename			ファイル名
 * @param fileSize [out]	読み込んだファイルサイズ
 * @return					ファイルのバイト列（freeBytesで解放してね）
 */
char* File::readBytes(const char* filename, u32& fileSize)
{
// open file
	FILE* fp = fopen(filename, "rb");
	if(fp == NULL) throw invalid_argument(filename);
// get file size with seek
	if( fseek(fp, 0, SEEK_END) ) throw runtime_error("error at fseek().");
	int pos = ftell(fp);
	if(pos == -1) throw runtime_error("error at ftell().");
	fileSize = pos;
// allocate returning buffer
	char* buffer = new char[fileSize];

	if( fseek(fp, 0, SEEK_SET) ) throw runtime_error("error at fseek().");
	if( fread(buffer, sizeof(char), fileSize, fp) != fileSize ) {
		throw runtime_error("error at fread().");
	}

	return buffer;
}

/**
 * readBytesで読み込んだバイト列を解放
 * @param pointer		バイト列のポインタ
 */
void File::freeBytes(void* pointer)
{
	char* p = (char*)pointer;
	delete[] p;
}

/**
 * パス文字列からファイル名のみを取り出す
 * @param filename		ファイルパス
 * @return				ファイル名
 */
std::string File::getFileName(const std::string& filename)
{
	std::string::size_type pos = filename.find_last_of('/');
	return (pos != std::string::npos)? filename.substr(pos + 1) : filename;
}

/**
 * パス文字列から拡張子なしファイル名のみを取り出す
 * @param filename		ファイルパス
 * @return				ファイル名（拡張子なし）
 */
std::string File::getFileNameWithoutExtension(const std::string& filename)
{
	std::string::size_type pos0 = filename.find_last_of('/');
	std::string::size_type pos1 = filename.find_last_of('.');
	if (pos0 != std::string::npos) {
		return (pos1 != std::string::npos)? filename.substr(pos0 + 1, pos1 - pos0) : filename.substr(pos0 + 1);
	} else {
		return (pos1 != std::string::npos)? filename.substr(0, pos1) : filename;
	}
}

/**
 * パス文字列からディレクトリ名のみを取り出す
 * @param filename		ファイルパス
 * @return				ディレクトリ名
 */
std::string File::getDirectoryName(const std::string& filename)
{
	std::string::size_type pos = filename.find_last_of('/');
	return (pos != std::string::npos)? filename.substr(0, pos) : "";
}

/**
 * パス文字列から拡張子のみを取り出す
 * @param filename		ファイルパス
 * @return				拡張子
 */
std::string File::getExtension(const std::string& filename)
{
	std::string::size_type pos = filename.find_last_of('.');
	return (pos != std::string::npos)? filename.substr(pos + 1) : "";
}



/**
 * ディレクトリが存在します？
 * @param name			ディレクトリ名
 * @retval true			あります
 * @retval false		ありません
 */
bool Directory::exists(const char* name)
{
	DIR* dir = opendir(name);
	if(dir != NULL) {
		if( closedir(dir) != 0 ) throw runtime_error("Error at closedir.");
		return true;
	} else return false;
}

/**
 * ディレクトリの作成
 * @param name			ディレクトリ名
 * @retval true			作成成功
 * @retval false		作成失敗
 */
bool Directory::create(const char* name)
{
	return mkdir(name, 0755) == 0;
}

/**
 * ホームディレクトリパスを取得
 * @return				ホームディレクトリパス
 */
std::string Directory::getHomeDirectory()
{
	return string( getenv("HOME") );
/*
	NSString* homeDir = NSHomeDirectory();
	return std::string([homeDir UTF8String]);
 */
}

/**
 * ファイルリスト取得
 * @param dirName		ディレクトリ名
 * @return				ファイルリスト
 */
std::vector<std::string> Directory::getFiles(const char* dirName)
{
	return getContentsImpl(dirName, true, false);
}

/**
 * ディレクトリリスト取得
 * @param dirName		ディレクトリ名
 * @return				ディレクトリリスト
 */
std::vector<std::string> Directory::getDirectories(const char* dirName)
{
	return getContentsImpl(dirName, false, true);
}

/**
 * ファイル＆ディレクトリリスト取得
 * @param dirName		ディレクトリ名
  * @return				ファイル＆ディレクトリリスト
 */
std::vector<std::string> Directory::getContents(const char* dirName)
{
	return getContentsImpl(dirName, true, true);
}


/**
 * ファイル＆ディレクトリリスト取得 内部関数
 * @param dirName		ディレクトリ名
 * @param addFile		ファイルをリストに追加
 * @param addDirectory	ディレクトリをリストに追加
 * @return				ファイル＆ディレクトリリスト
 */
std::vector<std::string> Directory::getContentsImpl(const char* dirName, bool addFile, bool addDirectory)
{
	std::vector<std::string> files;

	DIR* dir = opendir(dirName);
	if(dir == NULL) throw invalid_argument(dirName);

	struct dirent* dp;
	while( (dp = readdir(dir)) != NULL ) {
		if(
			( (dp->d_type == DT_DIR) && addDirectory ) ||
			( (dp->d_type == DT_DIR) && addFile )
		) files.push_back(dp->d_name);
	}

	if( closedir(dir) != 0 ) throw runtime_error("Error at closedir.");

	return files;
}

}	// namespace kuto
