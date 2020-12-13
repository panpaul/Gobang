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
		kGameHumanComputer, ///< 人机对战 人执黑
		kGameHH,            ///< 人人对战
		kGameComputerHuman, ///< 人机对战 人执白
		kGameCC             ///< 电脑对战
	};

	explicit Game(QWidget* window);
	Game(QWidget* window, GameType version);
	void Invoke(Board::OP); ///< 执行一步操作
	void Next();            ///< 下一步操作(电脑响应)
	void Reset();           ///< 重置

 public:
	std::shared_ptr<Board> board;
 private:
	Engine* engine;
	QWidget* mainWindow;
	Board::PlayerInfo player;
	bool secondEnd = false;
	GameType version;
	int statistic = 0, times = 0;
};

#endif //GOBANG__GAME_H_
