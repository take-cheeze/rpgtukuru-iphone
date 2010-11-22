/*
 * outputs analyze result to ./BASENAME.lsd.txt

	$CXX -Wall -DRPG2K_DEBUG=1 -I../rpg2kLib \
		lsd_analze.cpp \
		../rpg2kLib/rpg2k/FileToCompile.cpp \
		../rpg2kLib/rpg2k/define/*.cpp
 */


#include <cstdlib>
#include <iostream>
#include <rpg2kLib.hpp>


int main(int argc, char* argv[])
{
	if(argc != 2) {
		std::cout << "usage: " << argv[0] << " 'SaveXX.lsd'" << std::endl;
		return EXIT_FAILURE;
	} else {
		std::string const filename = argv[1];
		std::size_t const pos = filename.rfind('/');
		rpg2k::debug::ANALYZE_RESULT.close();
		rpg2k::debug::ANALYZE_RESULT.open(
			( filename.substr( pos != std::string::npos? pos : 0 ) + ".txt" ).c_str() );
		rpg2k::model::SaveData(".", filename);
		return EXIT_SUCCESS;
	}
}
