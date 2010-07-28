/**
 * @file
 * @brief Fileアクセス
 * @author project.kuto
 */

#include <kuto/kuto_file.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <cstdio>
#include <cstdlib>

#include <dirent.h>
#include <sys/stat.h>

namespace kuto {

/**
 * ファイルが存在します？
 * @param filename		ファイル名
 * @retval true			存在するよ
 * @retval false		ありません
 */
bool File::exists(const char* filename0)
{
	std::string filename = utf82sjis(filename0);
	FILE *fp = fopen(filename.c_str(), "rb");
	if (fp) {
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
char* File::readBytes(const char* filename0, u32& fileSize)
{
	std::string filename = utf82sjis(filename0);
	/* fpos_t */ size_t fsize = 0;
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		fileSize = 0;
		return NULL;
	}
// get file size
	fseek(fp, 0, SEEK_END); 
	fsize = ftell(fp);
	// fgetpos(fp, &fsize); 
 	fileSize = fsize;
// read data
 	char* buf = new char[fsize];
 	fseek(fp, 0, SEEK_SET);
 	size_t readSize = fread(buf, 1, fsize, fp);
	readSize = readSize;

 	fclose(fp);
	return buf;
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
	if(dir) {
		closedir(dir);
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
#if RPG2K_IS_WINDOWS
	return ( mkdir(name) == 0 ) ? true : false;
#else
	return ( mkdir(name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0 ) ? true : false;
#endif
}

/**
 * ホームディレクトリパスを取得
 * @return				ホームディレクトリパス
 */
std::string Directory::getHomeDirectory()
{
#if RPG2K_IS_WINDOWS
	return std::string( getenv("HOMEDRIVE") ) + getenv("HOMEPATH");
#else
	return std::string( getenv("HOME") );
#endif
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

	if(dir) while (true) {
		dirent* entry;
		if ((entry = readdir(dir)) != NULL) {
			// std::cout << entry->d_name << std::endl;
			struct stat statbuf;
			if( 
				(stat(entry->d_name, &statbuf) == -1) && 
				(
					( addDirectory && (statbuf.st_mode & S_IFDIR) ) ||
					( addFile && !(statbuf.st_mode & S_IFDIR) )
				)
			) files.push_back(entry->d_name);
		} else {
			closedir(dir);
		}
	}

	return files;
}

}	// namespace kuto
