#ifndef _INC__RPG2K__MODEL__PROJECT__HPP
#define _INC__RPG2K__MODEL__PROJECT__HPP

#include "DataBase.hpp"
#include "Font.hpp"
#include "MapTree.hpp"
#include "MapUnit.hpp"
#include "SaveData.hpp"
#include "SpecialArray1D.hpp"

#include <boost/smart_ptr.hpp>

#include <deque>


namespace rpg2k
{
	class Main;

	namespace model
	{
		class MapUnit;
		class SaveData;

		namespace Wallpaper { enum Type { ZOOM = 0, TILE = 1, }; }

		class Project
		{
		private:
			SystemString baseDir_, rtpDir_;

			DataBase ldb_;
			MapTree  lmt_;
			std::deque< boost::shared_ptr<MapUnit > > lmu_;
			std::deque< boost::shared_ptr<SaveData> > lsd_;

			uint lastSaveDataID_;
			double lastSaveDataStamp_;
		protected:
			void init();
			Project(Project const& src);
			Project& operator =(Project const& src);
		public:
			Project(SystemString baseDir=".");

			RPG2kString const& gameTitle() const { return lmt_[0][1]; }

			SystemString const& gameDir() const { return baseDir_; }

			uint getCurrentMapID();

			DataBase const& getLDB() const { return ldb_; }
			 MapTree const& getLMT() const { return lmt_; }
			SaveData const& getLSD() const;

			structure::Array1D & getLMT(uint id) const { return lmt_[id]; }
			 MapUnit& getLMU(uint id);
			SaveData& getLSD(uint id);

			DataBase& getLDB() { return ldb_; }
			 MapTree& getLMT() { return lmt_; }
			 MapUnit& getLMU() { return getLMU( getCurrentMapID() ); }
			SaveData& getLSD();

			int lastLSD() const { return lastSaveDataID_; }
			double lastLSDStamp() const { return lastSaveDataStamp_; }

			int chipSetID();
			structure::Array1D& chipSet() { return getLDB().chipSet()[chipSetID()]; }
			RPG2kString panorama(); // if return is empty() it's disabled

			void newGame();

		// returns true if the character can level up
			bool canLevelUp(uint harID);

			void loadLSD(uint id);
			void saveLSD(uint id);

			void resetLSD() { getLSD().reset(); }

			bool canTeleport();
			bool canEscape  ();
			bool canSave    ();
			bool canOpenMenu() const;

			bool isAbove(int chipID);
			bool isBelow(int chipID);
			bool isCounter(int chipID);
			uint8_t getPass(int chipID);
			int getTerrainID(int chipID);

			void processAction(uint eventID, Action::Type act, std::vector< int > const& arg);

			// returns true if equip success, false if failed
			bool   equip(uint harID, uint itemID);
			void unequip(uint harID, Equip::Type type);

			int currentPageID(uint eventID);
			structure::Array1D* currentPage(structure::Array2D const& pages) const;

			RPG2kString const& systemGraphic() const;
			Wallpaper::Type wallpaperType() const;
			font::Face::Type fontType() const;

			void move(uint mapID, int x, int y);

			RPG2kString const& faceSet(uint harID) const;
			int faceSetPos(uint harID) const;
			RPG2kString const& charSet(uint harID) const;
			int charSetPos(uint harID) const;

			RPG2kString const& name(uint harID) const;
			void setName(uint harID, RPG2kString const& val);
			RPG2kString const& title(uint harID) const;
			void setTitle(uint harID, RPG2kString const& val);

			int level(uint harID) const;
			int levelExp(uint harID) const;
			int conditionID(uint harID) const;
			RPG2kString const& condition(uint harID) const;
			int conditionColor(uint harID) const;
			int hp(uint harID) const;
			int mp(uint harID) const;
			int param(uint harID, Param::Type t) const;
			int exp(uint harID) const;

			int nextLevelExp(uint harID) const;
			int exp(uint harID, uint evel) const;

			std::vector<uint> sortLSD() const;
		}; // class Project
	} // namespace model
} // namespace rpg2k

#endif
