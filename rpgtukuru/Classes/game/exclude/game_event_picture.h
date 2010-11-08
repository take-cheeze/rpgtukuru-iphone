/**
 * @file
 * @brief Event Picture
 * @autor project.kuto
 */
#pragma once

#include <string>
#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>

#include <rpg2k/Array1D.hpp>
#include <rpg2k/Element.hpp>
namespace rpg2k { namespace structure { class Instruction; } }

class GameMap;


class GameEventPicture : public kuto::IRender2D, public kuto::TaskCreatorParam2<GameEventPicture, std::string const&, rpg2k::structure::Array1D&>
{
	friend class kuto::TaskCreatorParam2<GameEventPicture, std::string const&, rpg2k::structure::Array1D&>;
public:
	enum EffectType {
		kEffectNone,
		kEffectRoll,
		kEffectWave,
	};
	/*
	struct Info {
		kuto::Vector2	position;
		GameMap*		map;
		bool			scroll;
		bool			useAlpha;
		EffectType		effect;
		int				effectSpeed;
		float			scale;
		kuto::Color		color;
		float			saturation;
	};
	 */

private:
	GameEventPicture(const std::string& filename, rpg2k::structure::Array1D& info);

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D* g) const;

	void move(rpg2k::structure::Instruction const& info, int count);
	bool isMoving() const { return info_[51].get<int>() > 0; }
	void setPriority(float value); // { priority_ = value; }

private:
	rpg2k::structure::Array1D&	info_;
	// Info						infoMove_;
	kuto::Texture				texture_;
	int							effectCounter_;
	int							moveCounter_;
	int							moveCounterMax_;
	float						priority_;
};
