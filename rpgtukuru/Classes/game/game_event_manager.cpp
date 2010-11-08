#include "game.h"
#include "game_chara_status.h"
#include "game_event_manager.h"
#include "game_event_command.h"
#include "game_field.h"
#include "game_map.h"
#include "game_map_object.h"
#include "game_message_window.h"
#include "game_name_input_menu.h"
#include "game_save_menu.h"
#include "game_select_window.h"
#include "game_shop_menu.h"

#include <rpg2k/Debug.hpp>
#include <rpg2k/Event.hpp>

#include <sstream>

using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;
using rpg2k::model::DataBase;
using rpg2k::model::Project;
using rpg2k::model::SaveData;


GameEventManager::GameEventManager(GameField& f)
: field_(f)
, messageWindow_( *addChild( GameMessageWindow::createTask( f.game() ) ) )
, nameInputMenu_( *addChild( GameNameInputMenu::createTask( f.game() ) ) )
, saveMenu_( *addChild( GameSaveMenu::createTask(f) ) )
, selectWindow_( *addChild( GameSelectWindow::createTask( f.game() ) ) )
, shopMenu_( *addChild( GameShopMenu::createTask(f) ) )
, skillAnime_(NULL)
, timer_(DEFUALT_TIMER_NUMBER)
, waiter_( *addChild( GameTimer::createTask() ) )
, current_(NULL)
, eventLeft_(false)
{
	{
		cache_.project = &field_.project();
		cache_.ldb = &cache_.project->getLDB();
		cache_.lsd = &cache_.project->getLSD();
		cache_.lmu = &cache_.project->getLMU();
	}

	messageWindow_.setPosition(kuto::Vector2(0.f, 160.f));
	messageWindow_.setSize(kuto::Vector2(320.f, 80.f));
	messageWindow_.setMessageAlign(GameMessageWindow::kAlignLeft);
	messageWindow_.pauseUpdate();

	selectWindow_.setPosition(kuto::Vector2(0.f, 160.f));
	selectWindow_.setSize(kuto::Vector2(320.f, 80.f));
	selectWindow_.setMessageAlign(GameMessageWindow::kAlignLeft);
	selectWindow_.pauseUpdate();

	nameInputMenu_.pauseUpdate();
	saveMenu_.pauseUpdate();
	shopMenu_.pauseUpdate();
	for(int i = 0; i < DEFUALT_TIMER_NUMBER; i++) {
		timer_[i] = addChild( GameTimer::createTask() );
		timer_[i]->pauseUpdate();
	}

	branchCode_.insert(2014); // select
	branchCode_.insert(2071); // battle
	branchCode_.insert(2072); // shop
	branchCode_.insert(2073); // inn
	branchCode_.insert(2201); // if() {}

	initCommandTable();

	setWait(false);
}
void GameEventManager::addContext(unsigned const evID, rpg2k::structure::Event const& ev, rpg2k::EventStart::Type const t)
{
	contextList_.insert( std::make_pair( t, new Context(*this, evID, t) ) )->second->start(ev);
}
void GameEventManager::updateCommonContext()
{
	Array2D& common = cache_.ldb->commonEvent();
	rpg2k::model::SaveData& lsd = *cache_.lsd;

	for(Array2D::Iterator it = common.begin(); it != common.end(); ++it) {
		if( !it->second->exists() ) continue;

		rpg2k::EventStart::Type t = rpg2k::EventStart::Type( (*it->second)[11].to<int>() );
		if(
			(t == rpg2k::EventStart::CALLED) ||
			( (*it->second)[12].to<bool>() && !lsd.flag( (*it->second)[13].to<int>() ) )
		) { continue; }

		addContext(0, (*it->second)[22].toEvent(), t);
	}
}
void GameEventManager::updateMapContext()
{
	SaveData& lsd = *cache_.lsd;
	GameMap& map = field_.map();

	if( map.justMoved() ) return;

	Array2D& eventStates = lsd.eventState();
	for(Array2D::Iterator it = eventStates.begin(); it != eventStates.end(); ++it) {
		if( !it->second->exists() ) continue;

		Array1D const& page = map.page( it->first );
		rpg2k::EventStart::Type t = rpg2k::EventStart::Type( page[33].to<int>() );
		switch(t) {
			case rpg2k::EventStart::AUTO:
			case rpg2k::EventStart::PARALLEL:
				addContext( it->first, page[52].toEvent(), t );
				break;
			default: break;
		}
	}

	#define PP_check(PRIORITY, function) \
		{ \
			std::stack<unsigned>& st = map.function(); \
			while( !st.empty() ) { \
				Array1D const& page = map.page( st.top() ); \
				addContext( st.top(), page[52].toEvent(), rpg2k::EventStart::PRIORITY ); \
				st.pop(); \
			} \
		}
	PP_check(EVENT_TOUCH, touchFromEvent)
	PP_check(PARTY_TOUCH, touchFromParty)
	PP_check(KEY_ENTER, keyEnter)
	#undef PP_check
}

bool GameEventManager::isWaiting() const
{
	return(
		( waiter_.left() > 0 ) ||
		( current_ && ( current_->waiter().left() > 0 ) )
	);
}
void GameEventManager::update()
{
	{
		cache_.project = &field_.project();
		cache_.ldb = &cache_.project->getLDB();
		cache_.lsd = &cache_.project->getLSD();
		cache_.lmu = &cache_.project->getLMU();
	}

	if( !isWaiting() && (eventLeft_ == false) ) {
		updateMapContext();
		updateCommonContext();
		eventLeft_ = true;
	} else if( !isWaiting() ) {
		current_->incrementPointer();
	}

	stepCounter_ = 0;

	for(ContextList::iterator it = contextList_.begin(); it != contextList_.end(); ++it) {
		it->second->update();

		if( it->second->stackEmpty() ) {
			delete it->second;
			contextList_.erase(it);
		}
	}

	if( contextList_.empty() ) {
		eventLeft_ = false;
	}
}
bool GameEventManager::execute(rpg2k::structure::Instruction const& inst)
{
	if( stepCounter_ >= unsigned(rpg2k::EV_STEP_MAX) ) return false;

	#if RPG2K_DEBUG
		rpg2k::debug::Tracer::printInstruction(inst, std::cout, true) << std::endl;
	#endif

	if( branchCode_.find( inst.code() / 10 ) != branchCode_.end() ) {
		current_->skipToEndOfJunction( inst.nest(), inst.code() );
	} else {
		CommandTable::const_iterator it = commandTable_.find( inst.code() );
		if( it != commandTable_.end() ) {
			( this->*(it->second) )(inst);
		}
	}

	stepCounter_++;

	return !isWaiting();
}
void GameEventManager::setWait(bool b)
{
	waiter_.setCount(b);
	waiter_.pauseUpdate();
	if(current_) {
		current_->waiter().setCount(b);
		current_->waiter().pauseUpdate();
	}
}
void GameEventManager::setWaitCount(unsigned c)
{
	waiter_.setCount(c);
	waiter_.pauseUpdate(false);
	current_->waiter().setCount(c);
	current_->waiter().pauseUpdate(false);
}
void GameEventManager::setCurrent(GameEventManager::Context& cont)
{
	current_ = &cont;
	cache_.lsd->setCurrentEventID( cont.eventID() );
}
void GameEventManager::waitProcess(GameEventManager::Context& cont)
{
	this->setCurrent(cont);

	CommandTable::const_iterator it = commandWaitTable_.find( cont.event()[cont.pointer()].code() );
	if( it != commandWaitTable_.end() ) {
		// TODO: execute moves in move command stack

		( this->*(it->second) )( cont.event()[cont.pointer()] );
	}
}

GameEventManager::Context::Context(GameEventManager& owner, unsigned const evID, rpg2k::EventStart::Type t)
: owner_(owner)
, waiter_( *owner.addChild( GameTimer::createTask() ) )
, eventID_(evID), type_(t)
{
}
GameEventManager::Context::~Context()
{
	waiter_.release();
}
bool GameEventManager::Context::isWaiting() const
{
	switch(type_) {
		case rpg2k::EventStart::KEY_ENTER:
		case rpg2k::EventStart::PARTY_TOUCH:
		case rpg2k::EventStart::EVENT_TOUCH:
		case rpg2k::EventStart::AUTO:
			if( owner_.isWaiting() ) {
				return true;
			}
			break;
		case rpg2k::EventStart::PARALLEL:
			if( waiter_.left() ) {
				return true;
			}
			break;
		case rpg2k::EventStart::CALLED: default:
			kuto_assert(false); // invalid
	}

	return false;
}
void GameEventManager::Context::start(rpg2k::structure::Event const& ev)
{
	clearCallStack();
	eventStack_.push( std::make_pair(&ev, 0) );
}
void GameEventManager::Context::call(rpg2k::structure::Event const& ev, Pointer const& p)
{
	kuto_assert( eventStack_.size() < unsigned(rpg2k::EV_STACK_MAX) );
	eventStack_.push( std::make_pair(&ev, p) );
}
void GameEventManager::Context::ret() // return
{
	eventStack_.pop();

	if( eventStack_.empty() ) {
		// TODO: clear move stack
	}
}
void GameEventManager::Context::clearCallStack()
{
	while( !eventStack_.empty() ) {
		eventStack_.pop();
	}
}
void GameEventManager::Context::update()
{
	if( isWaiting() ) {
		owner_.waitProcess(*this);
		return;
	}

	owner_.setCurrent(*this);
	while( !eventStack_.empty() ) {
		owner_.execute( event()[pointer()++]);
	}
}
void GameEventManager::Context::skipToEndOfJunction(unsigned const nest, unsigned const code)
{
	std::pair<rpg2k::structure::Event const*, Pointer>& cur = eventStack_.top();
	unsigned const codeAttrib = code % 10000 / 10;
	for( ; cur.second < cur.first->size(); ++cur.second) {
		if(
			( (*cur.first)[cur.second].nest() == nest ) ||
			( ( (*cur.first)[cur.second].code() % 10000 / 10 ) != codeAttrib )
		) { ++cur.second; return; }
	}
}
bool GameEventManager::Context::skipToElse(unsigned const nest, unsigned const code)
{
	std::pair<rpg2k::structure::Event const*, Pointer>& cur = eventStack_.top();

	for( ; cur.second < cur.first->size(); ++cur.second) {
		if( (*cur.first)[cur.second].nest() == nest ) {
			if( (*cur.first)[cur.second].code() == 10 ) {
				++cur.second;
				return false;
			} else if( (*cur.first)[cur.second].code() == code ) {
				++cur.second;
				return true;
			}
		}
	}
	return false;
}
void GameEventManager::Context::startLoop(Nest const& n)
{
	loopStack_.push( std::make_pair( n, pointer() ) );
}
void GameEventManager::Context::continueLoop()
{
	setPointer( loopStack_.top().second + 1 );
}
void GameEventManager::Context::breakLoop()
{
	skipToElse( loopStack_.top().first, CODE_LOOP_END );
	incrementPointer();
	loopStack_.pop();
}

void GameEventManager::openGameMassageWindow()
{
	Array1D const& lsdSys = cache_.lsd->system();

	messageWindow_.freeze(false);
	messageWindow_.open();
	messageWindow_.reset();
	messageWindow_.clearMessages();
	messageWindow_.setShowFrame( lsdSys[41].to<int>() != rpg2k::Window::NO_FRAME );
	messageWindow_.setPosition( kuto::Vector2( 0.f, 80.f * lsdSys[41].to<int>() ) );

	if( lsdSys[43].to<bool>() ) { // auto move
		// TODO
	}
}

void GameEventManager::openGameSelectWindow()
{
	Array1D const& lsdSys = cache_.lsd->system();

	selectWindow_.freeze(false);
	selectWindow_.open();
	selectWindow_.reset();
	selectWindow_.resetCursor();
	selectWindow_.clearMessages();
	selectWindow_.setShowFrame( lsdSys[41].to<int>() != rpg2k::Window::NO_FRAME );
	selectWindow_.setPosition( kuto::Vector2( 0.f, 80.f * lsdSys[41].to<int>() ) );

	if( lsdSys[43].to<bool>() ) { // auto move
		// TODO
	}
}

void GameEventManager::addLevelUpMessage(unsigned const charID, int const oldLevel)
{
	DataBase const& ldb = *cache_.ldb;
	Project const& proj = *cache_.project;
	Project::Character const& c = proj.character(charID);

	std::ostringstream oss( c.name().toSystem() );
	oss << "‚Í" << ldb.vocabulary(123).toSystem() << c.level() << ldb.vocabulary(36).toSystem();
	messageWindow_.addLine(oss.str());

	std::set<uint16_t> const& skillList = c.skill();
	Array2D const& charSkill = ldb.character()[charID][63];
	int level = 1;
	for(Array2D::ConstIterator it = charSkill.begin(); it != charSkill.end(); ++it) {
		if( !it->second->exists() ) continue;

		if( it->second->exists(1) ) level = (*it->second)[1];
		int skillID = (*it->second)[2].to<int>();
		if(level > oldLevel && level <= c.level()
		&& skillList.find(skillID) == skillList.end()
		) {
			messageWindow_.addLine(ldb.skill()[skillID][1].to_string().toSystem() + ldb.vocabulary(37).toSystem());
		}
	}
}

template<unsigned CODE>
void GameEventManager::addCommand()
{
	bool const res = commandTable_.insert( std::make_pair(
		CODE, &GameEventManager::command<CODE> ) ).second;
	kuto_assert(res);
}
template<unsigned CODE>
void GameEventManager::addCommandWait()
{
	bool const res = commandWaitTable_.insert( std::make_pair(
		CODE, &GameEventManager::commandWait<CODE> ) ).second;
	kuto_assert(res);
}
void GameEventManager::initCommandTable()
{
	addCommand<CODE_OPERATE_SWITCH>();
	addCommand<CODE_OPERATE_VAR>();
	addCommand<CODE_OPERATE_KEY>();
	addCommand<CODE_OPERATE_ITEM>();
	addCommand<CODE_OPERATE_INPUT>();
	addCommand<CODE_OPERATE_MONEY>();
	addCommand<CODE_OPERATE_TIMER>();

	addCommand<CODE_PARTY_CHANGE>();
	addCommand<CODE_PARTY_HP>();
	addCommand<CODE_PARTY_MP>();
	addCommand<CODE_PARTY_STATE>();
	addCommand<CODE_PARTY_SKILL>();
	addCommand<CODE_PARTY_REFRESH>();
	addCommand<CODE_PARTY_EXP>();
	addCommand<CODE_PARTY_LV>();
	addCommand<CODE_PARTY_POWER>();
	addCommand<CODE_PARTY_SOUBI>();
	addCommand<CODE_PARTY_DAMAGE>();
	addCommand<CODE_PARTY_NAME>();
	addCommand<CODE_PARTY_TITLE>();
	addCommand<CODE_PARTY_WALK>();
	addCommand<CODE_PARTY_FACE>();

	addCommand<CODE_SYSTEM_VEHICLE>();
	addCommand<CODE_SYSTEM_BGM>();
	addCommand<CODE_SYSTEM_SOUND>();
	addCommand<CODE_SYSTEM_GRAPHIC>();
	addCommand<CODE_SYSTEM_SCREEN>();

	addCommand<CODE_VEH_RIDE>();
	addCommand<CODE_VEH_LOCATE>();

	addCommand<CODE_EVENT_END>();
	// addCommand<CODE_BLOCK_END>();
	addCommand<CODE_IF_START>();

	addCommand<CODE_LOOP_START>();
	addCommand<CODE_LOOP_BREAK>();
	addCommand<CODE_LOOP_END>();
	addCommand<CODE_GOTO_MOVE>();
	addCommand<CODE_GOTO_LABEL>();

	addCommand<CODE_SELECT_START>();
	addCommand<CODE_SHOP>();
	addCommand<CODE_INN>();

	addCommand<CODE_EVENT_BREAK>();
	addCommand<CODE_EVENT_CLEAR>();
	addCommand<CODE_EVENT_GOSUB>();

	addCommand<CODE_MM_SOUND>();
	addCommand<CODE_MM_BGM_PLAY>();
	addCommand<CODE_MM_BGM_FADEOUT>();
	addCommand<CODE_MM_BGM_SAVE>();
	addCommand<CODE_MM_BGM_LOAD>();
	addCommand<CODE_MM_MOVIE>();

	addCommand<CODE_SCREEN_CLEAR>();
	addCommand<CODE_SCREEN_SHOW>();
	addCommand<CODE_SCREEN_COLOR>();
	addCommand<CODE_SCREEN_SHAKE>();
	addCommand<CODE_SCREEN_FLASH>();
	addCommand<CODE_SCREEN_SCROLL>();
	addCommand<CODE_SCREEN_WEATHER>();

	addCommand<CODE_PICT_SHOW>();
	addCommand<CODE_PICT_MOVE>();
	addCommand<CODE_PICT_CLEAR>();

	addCommand<CODE_CHARA_TRANS>();
	addCommand<CODE_CHARA_MOVE>();
	addCommand<CODE_CHARA_FLASH>();
	addCommand<CODE_MOVEALL_START>();
	addCommand<CODE_MOVEALL_CANCEL>();

	addCommand<CODE_LOCATE_MOVE>();
	addCommand<CODE_LOCATE_SAVE>();
	addCommand<CODE_LOCATE_LOAD>();

	addCommand<CODE_TXT_REM>();
	addCommand<CODE_TXT_REM_ADD>();
	addCommand<CODE_TXT_SHOW>();
	addCommand<CODE_TXT_SHOW_ADD>();
	addCommand<CODE_TXT_OPTION>();
	addCommand<CODE_TXT_FACE>();

	addCommand<CODE_NAME_INPUT>();
	addCommand<CODE_EVENT_LOCATE>();
	addCommand<CODE_EVENT_SWAP>();
	addCommand<CODE_LAND_ID>();
	addCommand<CODE_EVENT_ID>();
	addCommand<CODE_WAIT>();
	addCommand<CODE_CHIPSET>();
	addCommand<CODE_PANORAMA>();
	addCommand<CODE_ENCOUNT>();
	addCommand<CODE_CHIP_SWAP>();
	addCommand<CODE_TELEPORT>();
	addCommand<CODE_TELEPORT_PERM>();
	addCommand<CODE_ESCAPE>();
	addCommand<CODE_ESCAPE_PERM>();
	addCommand<CODE_SAVE_SHOW>();
	addCommand<CODE_SAVE_PERM>();
	addCommand<CODE_MENU_SHOW>();
	addCommand<CODE_MENU_PERM>();
	// addCommand<CODE_LABEL>();
	addCommand<CODE_GAMEOVER>();
	addCommand<CODE_TITLE>();
	addCommand<CODE_BTLANIME>();

	addCommand<CODE_BTL_GO_START>();
	addCommand<CODE_BTL_ANIME>();

	addCommand<CODE_2003_JOB>();
	addCommand<CODE_2003_BTL_CMD>();
	addCommand<CODE_2003_ATK_REPEAT>();
	addCommand<CODE_2003_ESCAPE100>();
	addCommand<CODE_2003_BTL_COMMON>();


	addCommandWait<CODE_LOCATE_MOVE>();
	commandWaitTable_[CODE_LOCATE_LOAD] = &GameEventManager::commandWait<CODE_LOCATE_MOVE>;
	addCommandWait<CODE_TXT_SHOW>();
	addCommandWait<CODE_BTL_GO_START>();
	addCommandWait<CODE_SELECT_START>();
	addCommandWait<CODE_WAIT>();
	addCommandWait<CODE_PICT_MOVE>();
	addCommandWait<CODE_SCREEN_SCROLL>();
	commandWaitTable_[CODE_PARTY_EXP] = &GameEventManager::commandWait<CODE_TXT_SHOW>;
	commandWaitTable_[CODE_PARTY_LV] = &GameEventManager::commandWait<CODE_TXT_SHOW>;
	addCommandWait<CODE_NAME_INPUT>();
	addCommandWait<CODE_OPERATE_KEY>();
	addCommandWait<CODE_INN>();
	addCommandWait<CODE_SHOP>();
	addCommandWait<CODE_SCREEN_COLOR>();
	addCommandWait<CODE_BTLANIME>();
	addCommandWait<CODE_SAVE_SHOW>();
	addCommandWait<CODE_MENU_SHOW>();
}
