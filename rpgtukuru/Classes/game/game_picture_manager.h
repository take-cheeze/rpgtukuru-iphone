#pragma once

#include <kuto/kuto_array.h>
#include <kuto/kuto_irender.h>

class GameField;


class GamePictureManager : public kuto::Task, public kuto::TaskCreatorParam1<GamePictureManager, GameField&>
{
	friend class kuto::TaskCreatorParam1<GamePictureManager, GameField&>;
private:
	GameField& field_;

	class Picture : public kuto::IRender2D, public kuto::TaskCreatorParam2<Picture, GameField&, unsigned const>
	{
		friend class kuto::TaskCreatorParam2<Picture, GameField&, unsigned const>;
	public:
		enum EffectType {
			kEffectNone,
			kEffectRoll,
			kEffectWave,
		};

		bool isMoving() const;

	private:
		Picture(GameField& f, unsigned const id);

		virtual bool initialize()
		{
			return isInitializedChildren();
		}
		virtual void update();
		virtual void render(kuto::Graphics2D& g) const;

	private:
		GameField& field_;
		unsigned const id_;
		unsigned effectCounter_;
	}; // class Picture
	kuto::Array<Picture*, rpg2k::PICTURE_NUM> pictureList_;

private:
	GamePictureManager(GameField& f);

	virtual void update();

public:
	bool isMoving(unsigned const id) const;
}; // class GamePictureManager
