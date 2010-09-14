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
namespace rpg2k { namespace model { class Project; } }


class GameShopMenu : public kuto::Task, public kuto::TaskCreatorParam1<GameShopMenu, rpg2k::model::Project&>
{
	friend class kuto::TaskCreatorParam1<GameShopMenu, rpg2k::model::Project&>;
public:
	enum State {
		kStateSelectBuyOrSell,
		kStateBuyItem,
		kStateSellItem,
		kStateBuyItemNum,
		kStateSellItemNum,
		kStateClosed,
	};

	bool closed() const { return state_ == kStateClosed; }
	bool buyOrSell() const { return buyOrSell_; }
	void setShopData(int shopType, int mesType, const std::vector<int>& items);

private:
	GameShopMenu(rpg2k::model::Project& gameSystem);

	virtual bool initialize();
	virtual void update();

	void setState(State newState, bool thanks = false);
	void updateDescriptionMessage();

private:
	rpg2k::model::Project&				gameSystem_;
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
