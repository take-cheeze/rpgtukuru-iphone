/**
 * @file
 * @brief ショップ
 * @author project.kuto
 */
#pragma once

#include <vector>
#include <kuto/kuto_task.h>

class GameSelectWindow;
class GameMessageWindow;
class GameSystem;


class GameShopMenu : public kuto::Task
{
public:
	enum State {
		kStateSelectBuyOrSell,
		kStateBuyItem,
		kStateSellItem,
		kStateBuyItemNum,
		kStateSellItemNum,
		kStateClosed,
	};
	static GameShopMenu* createTask(kuto::Task* parent, GameSystem& gameSystem) { return new GameShopMenu(parent, gameSystem); }

	bool closed() const { return state_ == kStateClosed; }
	bool buyOrSell() const { return buyOrSell_; }
	void setShopData(int shopType, int mesType, const std::vector<int>& items);
	
private:
	GameShopMenu(kuto::Task* parent, GameSystem& gameSystem);
	virtual ~GameShopMenu();
	
	virtual bool initialize();
	virtual void update();
	
	void setState(State newState, bool thanks = false);
	void updateDescriptionMessage();
	
private:
	GameSystem&				gameSystem_;
	GameSelectWindow*		buySellSelectWindow_;
	GameSelectWindow*		itemSelectWindow_;
	GameMessageWindow*		descriptionWindow_;
	GameMessageWindow*		charaWindow_;
	GameMessageWindow*		inventoryWindow_;
	GameMessageWindow*		moneyWindow_;
	State					state_;
	bool					buyOrSell_;
	int						shopType_;
	int						messageType_;
	int						checkItem_;
	std::vector<int>		shopItems_;
	std::vector<int>		sellItems_;
};
