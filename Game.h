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
	explicit Game(QWidget* window);
	void Invoke(Board::OP);

 public:
	std::shared_ptr<Board> board;
	Engine* engine;
 private:
	QWidget* mainWindow;
	Board::PlayerInfo player;

};

#endif //GOBANG__GAME_H_
