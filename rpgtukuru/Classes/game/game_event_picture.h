/**
 * @file
 * @brief Event Picture
 * @autor project.kuto
 */
#pragma once

#include <string>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_irender.h>

class GameMap;


class GameEventPicture : public kuto::Task, public kuto::IRender
{
public:
	enum EffectType {
		kEffectNone,
		kEffectRoll,
		kEffectWave,
	};
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
public:
	static GameEventPicture* createTask(kuto::Task* parent, const std::string& filename, const Info& info) { return new GameEventPicture(parent, filename, info); }

private:
	GameEventPicture(kuto::Task* parent, const std::string& filename, const Info& info);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();
	
public:
	virtual void render();
	
	void move(const Info& info, int count);
	bool isMoving() const { return moveCounter_ > 0; }
	
private:
	Info						infoBase_;
	Info						infoMove_;
	kuto::Texture				texture_;
	int							effectCounter_;
	int							moveCounter_;
	int							moveCounterMax_;
};

