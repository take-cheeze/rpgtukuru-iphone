/**
 * @file
 * @brief Game Map
 * @author project.kuto
 */

#include "game_map.h"
#include "game_system.h"

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_virtual_pad.h>

#include <CRpgLdb.h>

#include <utility>


namespace
{
	typedef std::multimap< uint, uint >::const_iterator x_it;
	typedef std::map< uint, std::multimap< uint, uint > >::const_iterator y_it;
	typedef std::deque< std::map< uint, std::multimap< uint, uint > > >::const_iterator priority_it;

	static kuto::Vector2 const
		SCREEN_SIZE(320.f, 240.f), CHIP_SIZE(16.f, 16.f), CHAR_SIZE(24.f, 32.f),
		CHIP_NUM = SCREEN_SIZE / CHIP_SIZE;
} // anonymous namespace

GameMap::GameMap()
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 9.f)
, mapId_(0), animationCounter_(0)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
, enableScroll_(true), scrolled_(false), scrollRatio_(1.f)
, panoramaAutoScrollOffset_(0.f, 0.f)
{
}

bool GameMap::load(int mapIndex, rpg2k::model::Project& sys)
{
	gameSystem_ = &sys;
	mapId_ = mapIndex;
	if( !rpgLmu_.Init( mapIndex, sys.chipSet()[2].get_string().toSystem(), sys.gameDir().c_str() ) ){
		printf("error: cannot open Map%04d.lmu\n", mapIndex);
		return false;
	}
	CRpgUtil::LoadImage(chipSetTex_, std::string(gameSystem_->gameDir()).append("/ChipSet/").append( gameSystem_->chipSet()[2].get_string().toSystem() ), true);
	return true;
}

bool GameMap::initialize()
{
	return isInitializedChildren();
}

void GameMap::update()
{
	animationCounter_++;
	if (scrollRatio_ < 1.f) {
		scrollRatio_ = kuto::min(1.f, scrollRatio_ + scrollSpeed_);
		screenOffset_ = kuto::lerp(scrollBase_, scrollOffset_, scrollRatio_);
	}
	const kuto::Texture* panorama = rpgLmu_.GetPanoramaTexture();
	if (panorama) {
		if (gameSystem_->getLMU()[35].get<bool>()) panoramaAutoScrollOffset_.x += gameSystem_->getLMU()[36].get<int>();
		if (gameSystem_->getLMU()[37].get<bool>()) panoramaAutoScrollOffset_.y += gameSystem_->getLMU()[38].get<int>();
	}

	for( ; !touchFromParty_.empty(); touchFromParty_.pop() );
	for( ; !touchFromEvent_.empty(); touchFromEvent_.pop() );

	eventMap_.clear();
	eventMap_.resize(rpg2k::EventPriority::END);
	pageNum_.clear();

	rpg2k::model::Project& proj = *gameSystem_;
	rpg2k::model::SaveData& lsd = proj.getLSD();
	rpg2k::structure::Array2D& eventStates = lsd.eventState();
	rpg2k::structure::Array2D& mapEvents = proj.getLMU()[81];
// mapping events
	for(rpg2k::structure::Array2D::Iterator it = eventStates.begin(); it != eventStates.end(); ++it) {
		if( !it.second().exists() ) continue;

		int eventID = it.first(), priority, pageID;
		rpg2k::structure::Array1D& event = mapEvents[eventID];
		rpg2k::structure::EventState& state = reinterpret_cast<rpg2k::structure::EventState&>( it.second() );

		rpg2k::structure::Array1D* page = proj.currentPage(event[5]);
		if( page != NULL ) {
			pageID = page->getIndex();
			priority = (*page)[34];
		} else {
			pageID = rpg2k::INVALID_PAGE_ID;
			priority = rpg2k::EventPriority::BELOW;
		}

		pageNum_.insert( std::make_pair(eventID, pageID) );

		int x = state.exists(12) ? state.x() : event[2];
		int y = state.exists(13) ? state.y() : event[3];

		if( eventMap_[priority].find(y) == eventMap_[priority].end() ) {
			eventMap_[priority].insert( std::make_pair( y, std::multimap< uint, uint >() ) );
		}
		eventMap_[priority][y].insert( std::make_pair(x, eventID) );
	}
// mapping non-events
	for(uint i = rpg2k::EV_ID_PARTY; i <= rpg2k::EV_ID_AIRSHIP; i++) {
		rpg2k::structure::EventState& state = lsd.eventState(i);

		if( state.mapID() != lsd.eventState(rpg2k::EV_ID_PARTY).mapID() ) continue;

		int x = state.x(), y = state.y();

		if( eventMap_[rpg2k::EventPriority::CHAR].find(x) == eventMap_[rpg2k::EventPriority::CHAR].end() ) {
			eventMap_[rpg2k::EventPriority::CHAR].insert( std::make_pair( y, std::multimap< uint, uint >() ) );
		}
		eventMap_[rpg2k::EventPriority::CHAR][y].insert( std::make_pair(x, i) );

		if(i == rpg2k::EV_ID_PARTY) {
		}
	}
}

/*
void GameMap::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::Layer::OBJECT_2D, 10.f);
	kuto::RenderManager::instance()->addRender(this, kuto::Layer::OBJECT_2D, 8.f);
	renderCount_ = 0;
}
 */

void GameMap::render(kuto::Graphics2D* g) const
{
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(CHIP_SIZE * screenScale_);

	rpg2k::model::Project& proj = *gameSystem_;
	rpg2k::model::SaveData& lsd = proj.getLSD();
	rpg2k::model::MapUnit & lmu = proj.getLMU();
// Panorama
	const kuto::Texture* panorama = rpgLmu_.GetPanoramaTexture();
	if (panorama) {
		// TODO: draw only SCREEN_SIZE. some panorama is bigger than the SCREEN_SIZE.
		kuto::Vector2 pos = screenOffset_;
		if (gameSystem_->getLMU()[35].get<bool>()) pos.x += panoramaAutoScrollOffset_.x;
		if (gameSystem_->getLMU()[37].get<bool>()) pos.y += panoramaAutoScrollOffset_.y;
		kuto::Vector2 scale(panorama->getOrgWidth(), panorama->getOrgHeight());
		scale *= screenScale_;
		g->drawTexture(*panorama, pos, scale, color, true);
	}
// chip and events
	// mapping like [y][x]

	kuto::Point2 baseP;

	int scrll = lmu[11];
// horizon
	if( !(scrll&0x02) ) {
		if( lmu[2].get<uint>() <= (baseP[0] + CHIP_NUM[0]) )
			baseP.x = lmu[2].get<uint>() - CHIP_NUM[0];
		else if(baseP.x < 0) baseP.x = 0;
	}
// vertical
	if( !(scrll&0x01) ) {
		if( lmu[3].get<uint>() <= (baseP[1] + CHIP_NUM[1]) )
			baseP.y = lmu[3].get<uint>() - CHIP_NUM[1];
		else if(baseP.y < 0) baseP.y = 0;
	}

	kuto::Point2 it;
	for(it.y = 0; it.y < CHIP_NUM[1]; it.y++) {
		bool aboveLw[20], aboveUp[20];
		int lw[20], up[20];

		// below chips
		for(it.x = 0; it.x < CHIP_NUM[0]; it.x++) {
			kuto::Point2 cur = baseP + it;
			up[ it.x ] = lmu.chipIDUp(cur.x, cur.y);
			lw[ it.x ] = lmu.chipIDLw(cur.x, cur.y);
			aboveUp[ it.x ] = proj.isAbove(up[ it.x ]);
			aboveLw[ it.x ] = proj.isAbove(lw[ it.x ]);

			kuto::Vector2 itVec( float(it.x), float(it.y) );
			if(!aboveLw[ it.x ]) drawChip( CHIP_SIZE*itVec, lw[ it.x ]);
			if(!aboveUp[ it.x ]) drawChip( CHIP_SIZE*itVec, up[ it.x ]);
		}

		// event graphics
		for(priority_it prIt = eventMap_.begin(); prIt != eventMap_.end(); ++prIt) {
			if(
				( prIt->find(baseP[1]+it.y) == prIt->end() ) ||
				( prIt->find(baseP[1]+it.y)->second.size() == 0 )
			) continue;

			std::multimap< uint, uint > const& base = prIt->find(baseP[1]+it.y)->second;
			for(x_it x = base.begin(); x != base.end(); ++x) {
				if(
					(x->first < (uint)baseP.x) ||
					( (baseP.x + CHIP_NUM.x) <= x->first )
				) continue;


				rpg2k::structure::EventState& state = lsd.eventState(x->second);
				it.x = x->first - baseP.x;

				kuto::Vector2 itVec( float(it.x), float(it.y) );
				if( state.exists(73) ) {
					/* TODO:
					drawCharSet(
						state,
						CHIP_SIZE * itVec - kuto::Vector2D( (CHAR_SIZE.x-CHIP_SIZE.x)/2, CHAR_SIZE.y - CHIP_SIZE.y )
					);
					 */
				} else drawChip( CHIP_SIZE * itVec, 10000+state.charSetPos() );
			}
		}
		// above chips
		for(it.x = 0; it.x < CHIP_NUM[0]; it.x++) {
			kuto::Vector2 itVec( float(it.x), float(it.y) );

			if(aboveLw[ it.x ]) drawChip( CHIP_SIZE * itVec, lw[ it.x ] );
			if(aboveUp[ it.x ]) drawChip( CHIP_SIZE * itVec, up[ it.x ] );
		}
	}
/*
// picture
// battle animation
	#if defined(RPG2000)
		drawPictures(g);
		drawBattles(g);
	#elif defined(RPG2000_VALUE) || defined(RPG2003)
		drawBattles(g);
		drawPictures(g);
	#endif
// massage window
// movie
 */
/*
	// g->drawTexture(chipSetTex_, kuto::Vector2(0, 0), kuto::Vector2(chipSetTex_.getWidth(), chipSetTex_.getHeight()), color, kuto::Vector2(0, 0), kuto::Vector2(1,1));
	if (renderCount_ == 0) {
		// Panorama
		const kuto::Texture* panorama = rpgLmu_.GetPanoramaTexture();
		if (panorama) {
			// TODO: draw only SCREEN_SIZE. some panorama is bigger than the SCREEN_SIZE.
			kuto::Vector2 pos = screenOffset_;
			if (gameSystem_->getLMU()[35].get<bool>()) pos.x += panoramaAutoScrollOffset_.x;
			if (gameSystem_->getLMU()[37].get<bool>()) pos.y += panoramaAutoScrollOffset_.y;
			kuto::Vector2 scale(panorama->getOrgWidth(), panorama->getOrgHeight());
			scale *= screenScale_;
			g->drawTexture(*panorama, pos, scale, color, true);
		}
		// Map Chip
		drawLowerChips(false);
		drawUpperChips(false);
	} else {
		drawLowerChips(true);
		drawUpperChips(true);
#if 0
		const rpg2k::structure::Array1D& chipSet = gameSystem_->getLDB().chipSet()[gameSystem_->getLMU()[1].get<int>()];
		for (int x = startX; x < rpgLmu_.GetWidth(); x++) {
			float posx = x * size.x + screenOffset_.x;
			if (posx >= SCREEN_SIZE.x)
				break;
			for (int y = startY; y < rpgLmu_.GetHeight(); y++) {
				float posy = y * size.y + screenOffset_.y;
				if (posy >= SCREEN_SIZE.y)
					break;
				kuto::Vector2 pos(posx, posy);
				int chipId = gameSystem_->getLMU().chipIDLw(x, y);
				int chipFlag = chipSet[4].getBinary()[chipId] & 0xFF;
				//int chipId = rpgLmu->.chipIDUp(x, y);
				//int chipFlag = chipSet[5].getBinary()[chipId] & 0xFF;
				char str[32];
				sprintf(str, "%02x", chipFlag);
				g->drawText(str, pos, color, 12.f, kuto::Font::TYPE_NORMAL);
			}
		}
#endif
	}
	const_cast<GameMap*>(this)->renderCount_++; // TODO
 */
}

void GameMap::drawLowerChips(bool high) const
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(CHIP_SIZE * screenScale_);
	int startX = kuto::max(0, (int)(-screenOffset_.x / size.x));
	int startY = kuto::max(0, (int)(-screenOffset_.y / size.y));
	// if ( !gameSystem_->getLDB().chipSet().exists((gameSystem_->getLMU()[1].get<int>()) ) ) return;

	// const rpg2k::structure::Array1D& chipSet = gameSystem_->getLDB().chipSet()[gameSystem_->getLMU()[1].get<int>()];
	CRpgLmu::TextureInfoSet infoSet;
	std::vector<DefferdCommand> defferedRenders;
	for (int x = startX; x < rpgLmu_.GetWidth(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if (posx >= SCREEN_SIZE.x)
			break;
		for (int y = startY; y < rpgLmu_.GetHeight(); y++) {
			float posy = y * size.y + screenOffset_.y;
			if (posy >= SCREEN_SIZE.y)
				break;
			kuto::Vector2 pos(posx, posy);
			drawChip( pos, gameSystem_->getLMU().chipIDLw(x, y) );
			/*
			if (rpgLmu_.GetLowerChip(x, y, animationCounter_, infoSet)) {
				if (infoSet.size > 0) {
					int chipId = gameSystem_->getLMU().chipIDLw(x, y);
					if (chipId >= (int)chipSet[4].getBinary().size())
						continue;
					if (((chipSet[4].getBinary()[chipId] & CRpgLdb::FLAG_CHARACTER_UP) != 0) == high) {
						if (infoSet.size == 1) {
							if (infoSet.info[0].texture == rpgLmu_.GetChipSetTexture()) {
								g->drawTexture(*infoSet.info[0].texture, pos, size, color,
									infoSet.info[0].texcoord[0], infoSet.info[0].texcoord[1]);
							} else {
								DefferdCommand com;
								com.info = infoSet.info[0];
								com.pos = pos;
								defferedRenders.push_back(com);
							}
						} else {
							for (int i = 0; i < infoSet.size; i++) {
								if (!infoSet.info[i].texture)
									continue;
								kuto::Vector2 offset((i % 2) * size.x * 0.5f, ((i % 4) / 2) * size.y * 0.5f);
								g->drawTexture(*infoSet.info[i].texture, pos + offset, size * 0.5f, color,
										infoSet.info[i].texcoord[0], infoSet.info[i].texcoord[1]);
							}
						}
					}
				}
			}
			 */
		}
	}
	for (uint i = 0; i < defferedRenders.size(); i++) {
		g->drawTexture(*defferedRenders[i].info.texture, defferedRenders[i].pos, size, color,
					   defferedRenders[i].info.texcoord[0], defferedRenders[i].info.texcoord[1]);
	}
}

uint FLAG_WALL = 1<<5;

void GameMap::drawUpperChips(bool high) const
{
	// kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	const kuto::Vector2 size(CHIP_SIZE * screenScale_);
	int startX = kuto::max(0, (int)(-screenOffset_.x / size.x));
	int startY = kuto::max(0, (int)(-screenOffset_.y / size.y));

	// if ( gameSystem_->getLDB().chipSet().exists(gameSystem_->getLMU()[1].get<int>()) ) return;

	// const rpg2k::structure::Array1D& chipSet = gameSystem_->getLDB().chipSet()[gameSystem_->getLMU()[1].get<int>()];
	CRpgLmu::TextureInfo info;
	for(int x = startX; x < rpgLmu_.GetWidth(); x++) {
		float posx = x * size.x + screenOffset_.x;
		if(posx >= 320.f)
			break;
		for(int y = startY; y < rpgLmu_.GetHeight(); y++) {
			float posy = y * size.y + screenOffset_.y;
			if (posy >= 240.f)
				break;
			kuto::Vector2 pos(posx, posy);
			drawChip( pos, gameSystem_->getLMU().chipIDUp(x, y) );
			/*
			if (rpgLmu_.GetUpperChip(x, y, info)) {
				uint chipId = gameSystem_->getLMU().chipIDUp(x, y);
				if (chipId < chipSet[5].getBinary().size()) {
					if (((chipSet[5].getBinary()[chipId] & CRpgLdb::FLAG_CHARACTER_UP) != 0) == high)
						g->drawTexture(*info.texture, pos, size, color, info.texcoord[0], info.texcoord[1]);
				}
			}
			 */
		}
	}
}

bool GameMap::isEnableMove(int nowX, int nowY, int nextX, int nextY) const
{
	if (nextX < 0 || nextY < 0)
		return false;
	if (nextX >= rpgLmu_.GetWidth() || nextY >= rpgLmu_.GetHeight())
		return false;
	const rpg2k::structure::Array1D& chipSet = gameSystem_->getLDB().chipSet()[gameSystem_->getLMU()[1].get<int>()];
	int nowFlag = 0, nextFlag = 0;
	if (nowX > nextX) {
		 nowFlag = rpg2k::EventDir::LEFT;
		nextFlag = rpg2k::EventDir::RIGHT;
	} else if (nowX < nextX) {
		 nowFlag = rpg2k::EventDir::RIGHT;
		nextFlag = rpg2k::EventDir::LEFT;
	} else if (nowY > nextY) {
		 nowFlag = rpg2k::EventDir::UP;
		nextFlag = rpg2k::EventDir::DOWN;
	} else if (nowY < nextY) {
		 nowFlag = rpg2k::EventDir::DOWN;
		nextFlag = rpg2k::EventDir::UP;
	}

	int upperChipNow = gameSystem_->getLMU().chipIDUp(nowX, nowY);
	int upperChipNext = gameSystem_->getLMU().chipIDUp(nextX, nextY);
	if (
		(upperChipNow != 0 && !gameSystem_->isCounter(upperChipNow)) ||
		(upperChipNext != 0 && !gameSystem_->isCounter(upperChipNext))
	) {
		if (
			(upperChipNow != 0 && (chipSet[5].getBinary()[upperChipNow] & nowFlag) == 0) ||
			(upperChipNext != 0 && (chipSet[5].getBinary()[upperChipNext] & nextFlag) == 0)
		) {
			bool noReturn = false;
			if (nextY > 0 && (chipSet[5].getBinary()[upperChipNext] & FLAG_WALL) != 0) {
				int upperChipUp = gameSystem_->getLMU().chipIDUp(nextX, nextY - 1);
				if (upperChipUp != upperChipNext)
					noReturn = true;
			}
			if ((chipSet[5].getBinary()[upperChipNow] & FLAG_WALL) != 0) {
				if (upperChipNow != upperChipNext)
					noReturn = true;
			}
			return noReturn;
		} else {
			return true;
		}
	}

	int lowerChipNow = gameSystem_->getLMU().chipIDLw(nowX, nowY);
	int lowerChipNext = gameSystem_->getLMU().chipIDLw(nextX, nextY);
	if ((chipSet[4].getBinary()[lowerChipNow] & nowFlag) == 0
	|| (chipSet[4].getBinary()[lowerChipNext] & nextFlag) == 0) {
		bool noReturn = false;
		if (nextY > 0 && (chipSet[4].getBinary()[lowerChipNext] & FLAG_WALL) != 0) {
			int lowerChipUp = gameSystem_->getLMU().chipIDLw(nextX, nextY - 1);
			if (lowerChipUp != lowerChipNext)
				noReturn = true;
		}
		if ((chipSet[4].getBinary()[lowerChipNow] & FLAG_WALL) != 0) {
			if (lowerChipNow != lowerChipNext)
				noReturn = true;
		}
		if (!noReturn)
			return false;
	}
	return true;
}

int GameMap::getChipFlag(int x, int y, bool upper) const
{
	const rpg2k::structure::Array1D& chipSet = gameSystem_->getLDB().chipSet()[gameSystem_->getLMU()[1].get<int>()];
	int chipId = upper? gameSystem_->getLMU().chipIDUp(x, y) : gameSystem_->getLMU().chipIDLw(x, y);
	return upper? chipSet[5].getBinary()[chipId] : chipSet[4].getBinary()[chipId];
}

bool GameMap::isCounter(int x, int y) const
{
	return gameSystem_->isCounter( gameSystem_->getLMU().chipIDUp(x, y) );
}


void GameMap::setPlayerPosition(const kuto::Vector2& pos)
{
	float mapWidth = rpgLmu_.GetWidth () * 16.f;
	float mapHeiht = rpgLmu_.GetHeight() * 16.f;
	if (pos.x < (320.f) * 0.5f)
		screenOffsetBase_.x = 0.f;
	else if (pos.x > mapWidth - (320.f) * 0.5f)
		screenOffsetBase_.x = (320.f) - mapWidth;
	else
		screenOffsetBase_.x = (320.f) * 0.5f - pos.x;
	if (pos.y < (240.f) * 0.5f)
		screenOffsetBase_.y = 0.f;
	else if (pos.y > mapHeiht - (240.f) * 0.5f)
		screenOffsetBase_.y = (240.f) - mapHeiht;
	else
		screenOffsetBase_.y = (240.f) * 0.5f - pos.y;
	if (!enableScroll_ || isScrolling() || scrolled_)
		return;
	screenOffset_ = screenOffsetBase_;
}

int GameMap::getTerrainId(int x, int y) const
{
	return gameSystem_->getTerrainID( gameSystem_->getLMU().chipIDLw(x, y) );
}

void GameMap::scroll(int x, int y, float speed)
{
	scrollOffset_.x = -x * 16.f;
	scrollOffset_.y = -y * 16.f;
	scrollBase_ = screenOffset_;
	scrollOffset_ += screenOffset_;
	scrollSpeed_ = speed / (std::abs(scrollOffset_.x - scrollBase_.x) + std::abs(scrollOffset_.y - scrollBase_.y));
	scrollRatio_ = 0.f;
	scrolled_ = true;
}

void GameMap::scrollBack(float speed)
{
	scrollOffset_ = screenOffsetBase_;
	scrollBase_ = screenOffset_;
	scrollSpeed_ = speed / (std::abs(scrollOffset_.x - scrollBase_.x) + std::abs(scrollOffset_.y - scrollBase_.y));
	scrollRatio_ = 0.f;
	scrolled_ = false;
}

void GameMap::drawChip(kuto::Vector2 const& dstP, int const chipID) const
{
	int const interval = kuto::random(3*4); // getOwner().loopCount() % (3*4);

	kuto::Graphics2D* const g = kuto::RenderManager::instance()->getGraphics2D();

	kuto::Texture const& src = chipSetTex_;
	rpg2k::model::SaveData& lsd = gameSystem_->getLSD();

	kuto::Vector2 srcP;

	rpg2k_assert( src.isValid() );

	if( chipID == 10000 ) { return; // skip unvisible upper chip
	} else if( rpg2k::within(chipID, 3000) ) {
		rpg2k_assert( rpg2k::within( chipID / 1000, 3) );
		rpg2k_assert( rpg2k::within( chipID % 1000 / 50, 0x10 ) );
		rpg2k_assert( rpg2k::within( chipID % 1000 % 50, 0x2f ) );

		drawBlockA_B(src, dstP, chipID / 1000, chipID % 1000 / 50,  chipID % 1000 / 50, interval / 3);
		return;
	} else if( rpg2k::within(3000, chipID, 4000) ) {
		rpg2k_assert( rpg2k::within(interval, 4) );
		rpg2k_assert( ( (chipID-3000)%50 ) == 28 );

		srcP.set( (chipID-3000)/50+3, interval % 4 );
	} else if( rpg2k::within(4000, chipID, 5000) ) {
		rpg2k_assert( rpg2k::within( (chipID-4000) / 50, 12 ) );
		rpg2k_assert( rpg2k::within( (chipID-4000) % 50, 0x2f ) );

		drawBlockD( src, dstP, (chipID-4000) / 50, (chipID-4000) % 50 );
		return;
	} else if( rpg2k::within(5000, chipID, 5144) ) {
		int num = lsd.getReplace(rpg2k::ChipSet::LOWER, chipID -  5000);
		srcP.set(
			(num%6) + (num<96 ? 0 :  6) + 12,
			(num/6) - (num<96 ? 0 : 16)
		);
	} else if( rpg2k::within(10000, chipID, 10144) ) {
		int num = lsd.getReplace(rpg2k::ChipSet::UPPER, chipID - 10000);
		srcP.set(
			(num%6) + (num<48 ? 0 : 6) + (12+6),
			(num/6) + (num<48 ? 8 : -8)
		);
	} else rpg2k_assert(false);

	kuto::Vector2 const scale( src.getOrgWidth(), src.getOrgHeight() );
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	g->drawTexture(src, dstP, CHIP_SIZE, color, srcP * CHIP_SIZE / scale, (srcP+1) * CHIP_SIZE / scale);
}
// SEG[0] = (0,0), SEG[1] = (8,0),
// SEG[3] = (0,8), SEG[2] = (8,8)
static kuto::Vector2 const SEG_P[4] = {
	kuto::Vector2(0, 0),
	kuto::Vector2(8, 0),
	kuto::Vector2(8, 8),
	kuto::Vector2(0, 8),
};
void GameMap::drawBlockA_B(kuto::Texture const& src, kuto::Vector2 const& dstP, int const x, int const y, int const z, int const anime) const
{
	// OCN_SEG[0] = (0,0), OCN_SEG[1] = (8,0),
	// OCN_SEG[2] = (0,8), OCN_SEG[3] = (8,8)
	static kuto::Vector2 const OCN_SEG_P[4] = {
		kuto::Vector2(0, 0), kuto::Vector2(8, 0),
		kuto::Vector2(0, 8), kuto::Vector2(8, 8),
	};

	enum Pattern { A, B, C, D, N, };
	static kuto::Vector2 const PAT_P[4] = {
		kuto::Vector2(0, 16*0), // A
		kuto::Vector2(0, 16*1), // B
		kuto::Vector2(0, 16*2), // C
		kuto::Vector2(0, 16*3), // D
	};
// Ocean
	kuto::Vector2 const oceanP = CHIP_SIZE * kuto::Vector2(anime, 4);
	Pattern ocean[4];
	Pattern ON = (x == 2) ? D : A, OFF = (x == 2) ? C : B;

	for(int i = 0; i < 4; i++) ocean[i] = y&(0x01<<i) ? ON : OFF;
// Coast
	kuto::Vector2 const coastP( CHIP_SIZE[0]*(anime + (x==1 ? 3 : 0)), 0 );
	Pattern coast[4];
	if(z<0x10) {
		for(int i = 0; i < 4; i++) coast[i] = z&(0x01<<i) ? D : N;
	} else if(z<0x14) {
		coast[0] = B; coast[1] = z&0x01 ? D : N;
		coast[3] = B; coast[2] = z&0x02 ? D : N;
	} else if(z<0x18) {
		coast[0] = C; coast[1] = C;
		coast[3] = z&0x02 ? D : N; coast[2] = z&0x01 ? D : N;
	} else if(z<0x1c) {
		coast[0] = z&0x02 ? D : N; coast[1] = B;
		coast[3] = z&0x01 ? D : N; coast[2] = B;
	} else if(z<0x20) {
		coast[0] = z&0x01 ? D : N; coast[1] = z&0x02 ? D : N;
		coast[3] = C; coast[2] = C;
	} else if(z==0x21) {
		coast[0] = coast[1] = coast[3] = coast[2] = B;
	} else if(z==0x22) {
		coast[0] = coast[1] = coast[3] = coast[2] = C;
	} else if(z<0x24) {
		coast[0] = A; coast[1] = C;
		coast[3] = B; coast[2] = z&0x01 ? D : N;
	} else if(z<0x26) {
		coast[0] = C; coast[1] = A;
		coast[3] = z&0x01 ? D : N; coast[2] = B;
	} else if(z<0x28) {
		coast[0] = z&0x01 ? D : N; coast[1] = B;
		coast[3] = C; coast[2] = A;
	} else if(z<0x2a) {
		coast[0] = B; coast[1] = z&0x01 ? D : N;
		coast[3] = A; coast[2] = C;
	} else if(z==0x2a) {
		coast[0] = A; coast[1] = A;
		coast[3] = B; coast[2] = B;
	} else if(z==0x2b) {
		coast[0] = A; coast[1] = C;
		coast[3] = A; coast[2] = C;
	} else if(z==0x2c) {
		coast[0] = B; coast[1] = B;
		coast[3] = A; coast[2] = A;
	} else if(z==0x2d) {
		coast[0] = C; coast[1] = A;
		coast[3] = C; coast[2] = A;
	} else if(z==0x2e) {
		coast[0] = coast[1] =
		coast[3] = coast[2] = A;
	} else rpg2k_assert(false);
// Draw
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 const scale( src.getOrgWidth(), src.getOrgHeight() );
	for(int i = 0; i < 4; i++) {
		if( ocean[i] != N ) g->drawTexture(
			src, dstP + OCN_SEG_P[i], CHIP_SIZE / 2, color,
			(oceanP + PAT_P[ ocean[i] ] + OCN_SEG_P[i]) / scale,
			(oceanP + PAT_P[ ocean[i] ] + OCN_SEG_P[i] + CHIP_SIZE / 2) / scale
		);
	}
	for(int i = 0; i < 4; i++) {
		g->drawTexture(
			src, dstP + SEG_P[i], CHIP_SIZE / 2, color,
			(coastP + PAT_P[ coast[i] ] + SEG_P[i]) / scale,
			(coastP + PAT_P[ coast[i] ] + SEG_P[i] + CHIP_SIZE / 2) / scale
		);
	}
}
void GameMap::drawBlockD(kuto::Texture const& src, kuto::Vector2 const& dstP, int const x, int const y) const
{
	enum Pattern {
		A , B , C ,
		D7, D8, D9,
		D4, D5, D6,
		D1, D2, D3,
	};
	static kuto::Vector2 const PAT_P[12] = {
		kuto::Vector2( 0,  0), kuto::Vector2(16,  0), kuto::Vector2(32,  0),
		kuto::Vector2( 0, 16), kuto::Vector2(16, 16), kuto::Vector2(32, 16),
		kuto::Vector2( 0, 32), kuto::Vector2(16, 32), kuto::Vector2(32, 32),
		kuto::Vector2( 0, 48), kuto::Vector2(16, 48), kuto::Vector2(32, 48),
	};

	kuto::Vector2 const baseP(
		CHIP_SIZE[0]*(3*(x%2) + (x<4 ? 0 : 6)),
		CHIP_SIZE[1]*(4*(x/2) - (x<4 ? 0 : 8))
	);
	Pattern pat[4] = { D5, D5, D5, D5 };

	if(y < 0x10) {
		for(int i = 0; i < 4; i++) pat[i] = y&(0x01<<i) ? C : D5;
	} else if(y < 0x14) {
		pat[0] = D4; pat[1] = y&0x01 ? C : D5;
		pat[3] = D4; pat[2] = y&0x02 ? C : D5;
	} else if(y < 0x18) {
		pat[0] = D8; pat[1] = D8;
		pat[3] = y&0x02 ? C : D5; pat[2] = y&0x01 ? C : D5;
	} else if(y < 0x1C) {
		pat[0] = y&0x01 ? C : D5; pat[1] = D6;
		pat[3] = y&0x02 ? C : D5; pat[2] = D6;
	} else if(y < 0x20) {
		pat[0] = y&0x01 ? C : D5; pat[1] = y&0x02 ? C : D5;
		pat[3] = D2; pat[2] = D2;
	} else if(y == 0x20) {
		pat[0] = D4; pat[1] = D6;
		pat[3] = D4; pat[2] = D6;
	} else if(y == 0x21) {
		pat[0] = D8; pat[1] = D8;
		pat[3] = D2; pat[2] = D2;
	} else if(y < 0x24) {
		pat[0] = D7; pat[1] = D7;
		pat[3] = D7; pat[2] = y&0x01 ? C : D7;
	} else if(y < 0x26) {
		pat[0] = D9; pat[1] = D9;
		pat[3] = y&0x01 ? C : D9; pat[2] = D9;
	} else if(y < 0x28) {
		pat[0] = y&0x01 ? C : D3; pat[1] = D3;
		pat[3] = D3; pat[2] = D3;
	} else if(y < 0x2a) {
		pat[0] = D1; pat[1] = y&0x01 ? C : D1;
		pat[3] = D1; pat[2] = D1;
	} else if(y == 0x2a) {
		pat[0] = D7; pat[1] = D9;
		pat[3] = D7; pat[2] = D9;
	} else if(y == 0x2b) {
		pat[0] = D7; pat[1] = D7;
		pat[3] = D1; pat[2] = D1;
	} else if(y == 0x2c) {
		pat[0] = D1; pat[1] = D3;
		pat[3] = D1; pat[2] = D3;
	} else if(y == 0x2d) {
		pat[0] = D9; pat[1] = D9;
		pat[3] = D3; pat[2] = D3;
	} else if(y == 0x2e) {
		pat[0] = D7; pat[1] = D9;
		pat[3] = D1; pat[2] = D3;
	} else rpg2k_assert(false);

	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	kuto::Color const color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 const scale( src.getOrgWidth(), src.getOrgHeight() );
	for(int i = 0; i < 4; i++) {
		g->drawTexture(
			src, dstP+SEG_P[i], CHIP_SIZE/2, color,
			(baseP + PAT_P[ pat[i] ] + SEG_P[i]) / scale,
			(baseP + PAT_P[ pat[i] ] + SEG_P[i] + CHIP_SIZE/2) / scale
		);
	}
}
