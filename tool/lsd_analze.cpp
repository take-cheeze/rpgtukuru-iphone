/*
	$CXX -Wall -DRPG2K_DEBUG=1 -I../rpg2kLib \
		lsd_analze.cpp \
		../rpg2kLib/rpg2k/*.cpp \
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
		rpg2k::model::SaveData(".", argv[1]);
		return EXIT_SUCCESS;
	}
}
