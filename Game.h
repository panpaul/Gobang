/**
 * @file Game.h
 * @brief 游戏主类
 * @author Paul
 */

#ifndef GOBANG__GAME_H_
#define GOBANG__GAME_H_

#include "Board.h"
#include "Engine.h"
#include <QMessageBox>

class Game : QObject
{
 public:
	enum GameType
	{
		kGameHumanComputer,
		kGameHH,
		kGameComputerHuman,
		kGameCC
	};

	explicit Game(QWidget* window);
	Game(QWidget* window, GameType version);
	void Invoke(Board::OP);
	void Next();
	void Reset();

 public:
	std::shared_ptr<Board> board;
	Engine* engine;
 private:
	QWidget* mainWindow;
	Board::PlayerInfo player;
	bool secondEnd = false;
	GameType version;

};

#endif //GOBANG__GAME_H_
