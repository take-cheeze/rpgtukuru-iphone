/**
 * @file
 * @brief Fileアクセス
 * @author project.kuto
 */

#include "kuto_file.h"
#include "kuto_error.h"
#import <UIKit/UIKit.h>


namespace kuto {

/**
 * ファイルが存在します？
 * @param filename		ファイル名
 * @retval true			存在するよ
 * @retval false		ありません
 */
bool File::exists(const char* filename)
{
	NSString* path = [NSString stringWithUTF8String:filename];
	BOOL isDir = NO;
	if ([[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir])
		return isDir == NO? true : false;
	return false;
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
	NSString* path = [NSString stringWithUTF8String:filename];
	NSData* data = [NSData dataWithContentsOfFile:path];
	fileSize = [data length];
	char* buffer = new char[fileSize];
	[data getBytes:buffer];
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
	NSString* path = [NSString stringWithUTF8String:name];
	BOOL isDir = NO;
	if ([[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir])
		return isDir == YES? true : false;
	return false;
}

/**
 * ディレクトリの作成
 * @param name			ディレクトリ名
 * @retval true			作成成功
 * @retval false		作成失敗
 */
bool Directory::create(const char* name)
{
	NSString* path = [NSString stringWithUTF8String:name];
	if ([[NSFileManager defaultManager] createDirectoryAtPath:path attributes:nil])
		return true;
	return false;
}

/**
 * ホームディレクトリパスを取得
 * @return				ホームディレクトリパス
 */
std::string Directory::getHomeDirectory()
{
	NSString* homeDir = NSHomeDirectory();
	return std::string([homeDir UTF8String]);
}

}	// namespace kuto
