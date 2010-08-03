/**
 * @file
 * @brief Game BGM (Sample)
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>

class GameField;


class GameBgm : public kuto::Task
{
public:
	static GameBgm* createTask(kuto::Task* parent, const char* filename) { return new GameBgm(parent, filename); }

private:
	GameBgm(kuto::Task* parent, const char* filename);

	virtual void update();

public:
	void play() { playing_ = true; }
	void pause() { playing_ = false; }
	void stop() { playing_ = false; count_ = 0; }
	bool isPlaying() const { return playing_; }
	bool isLooped() const { return count_ > endCount_; }
	int count() const { return count_; }
	int	endCount() const { return endCount_; }

private:
	int			count_;
	int			endCount_;
	bool		playing_;
};

