/**
 * @file
 * @brief Game System Menu Base Class
 * @author project.kuto
 */

#include "game_system_menu_base.h"
#include "game_field.h"


GameSystemMenuBase::GameSystemMenuBase(GameField* gameField)
: kuto::Task(gameField)
, gameField_(gameField), state_(kStateNone)
{
}

GameSystemMenuBase::~GameSystemMenuBase()
{
}
