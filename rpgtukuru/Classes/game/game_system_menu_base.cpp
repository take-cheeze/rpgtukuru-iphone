/**
 * @file
 * @brief Game System Menu Base Class
 * @author project.kuto
 */

#include "game_system_menu_base.h"
#include "game_field.h"


GameSystemMenuBase::GameSystemMenuBase(GameField& gameField)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 20.f)
, field_(gameField), state_(kStateNone)
{
}
