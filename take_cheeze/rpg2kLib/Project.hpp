#ifndef _INC__RPG2K__PROJECT_HPP
#define _INC__RPG2K__PROJECT_HPP

#include "DataBase.hpp"
#include "MapTree.hpp"
#include "MapUnit.hpp"
#include "SaveData.hpp"

namespace rpg2kLib
{
	class Main;

	namespace model
	{

		class Project
		{
		private:
			string BASE_DIR, RTP_DIR;

			DataBase LDB;
			MapTree  LMT;
			Map< uint, MapUnit  > LMU;
			Map< uint, SaveData > LSD;

			uint LAST_LSD_ID;
			uint64_t LAST_SAVEDATA_STAMP;
		protected:
			void init();
		public:
			Project(string baseDir=".", string rtpDir=".");
			// Project(Main& m);
			~Project();

			string getGameTitle() { return LMT[0][1]; }

			string getGameDir() { return BASE_DIR; }
			string getRTPDir () { return  RTP_DIR; }

			uint getCurrentMapID();

			DataBase& getLDB() { return LDB; }
			Array1D & getLMT();
			MapUnit & getLMU();
			SaveData& getLSD();

			Array1D & getLMT(uint id) { return LMT[id]; }
			MapUnit & getLMU(uint id);
			SaveData& getLSD(uint id);

			int lastLSD() { return LAST_LSD_ID; }

			int chipSetID() { return getLMU()[1]; }

			void loadLSD(uint id);
			void saveLSD(uint id);

			bool canTeleport();
			bool canEscape();
			bool canSave();
			bool canOpenMenu();

			bool isAbove(uint chipID);
			bool isBelow(uint chipID);
			bool isCounter(uint chipID);
			uint8_t getPass(uint chipID);
			int getTerrainID(uint chipID);

			// returns true if equip success, false if failed
			bool equip(uint charID, uint itemID);
			void unequip(uint charID, EquipType type);

			uint currentPageID(uint eventID);

			string systemGraphic();
			uint wallpaperType();
			uint fontType();
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
