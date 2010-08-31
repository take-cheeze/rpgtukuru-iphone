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
			uint64_t lastSaveDataStamp_;
		protected:
			void init();
			Project(Project const& src);
			Project& operator =(Project const& src);
		public:
			Project(SystemString baseDir=".");
			~Project();

			RPG2kString gameTitle() const { return lmt_[0][1]; }

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
			uint64_t lastLSDStamp() const { return lastSaveDataStamp_; }

			int chipSetID();
			structure::Array1D& chipSet() { return getLDB().chipSet()[chipSetID()]; }
			RPG2kString panorama();

			void newGame();

		// returns true if the character can level up
			bool canLevelUp(uint const charID);

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
			bool   equip(uint const charID, uint itemID);
			void unequip(uint const charID, Equip::Type type);

			int currentPageID(uint eventID);
			structure::Array1D* currentPage(structure::Array2D const& pages) const;

			RPG2kString const& systemGraphic() const;
			Wallpaper::Type wallpaperType() const;
			font::Face::Type fontType() const;

			void move(uint mapID, int x, int y);

			RPG2kString faceSet(uint const charID) const;
			int faceSetPos(uint const charID) const;
			RPG2kString charSet(uint const charID) const;
			int charSetPos(uint const charID) const;

			RPG2kString name(uint const charID) const;
			void setName(uint const charID, RPG2kString const& val);
			RPG2kString title(uint const charID) const;
			void setTitle(uint const charID, RPG2kString const& val);

			int level(uint const charID) const;
			int levelExp(uint const charID) const;
			int conditionID(uint const charID) const;
			RPG2kString condition(uint const charID) const;
			int conditionColor(uint const charID) const;
			int hp(uint const charID) const;
			int mp(uint const charID) const;
			int param(uint const charID, Param::Type t) const;
			int exp(uint const charID) const;

			int nextLevelExp(uint const charID) const;
			int exp(uint const charID, uint const level) const;
		}; // class Project
	} // namespace model
} // namespace rpg2k

#endif
