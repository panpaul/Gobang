#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "Game.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class GameWindow;
}
QT_END_NAMESPACE

class GameWindow : public QMainWindow
{
 Q_OBJECT

 public:
	explicit GameWindow(QWidget* parent);
	GameWindow(QWidget* parent, Game::GameType type);
	~GameWindow() override;

 private:
	Ui::GameWindow* ui;
	std::shared_ptr<Game> game;
	std::shared_ptr<Board> board;
	bool allowDraw = true;
	Game::GameType gameType;

 private:
	int mouseX = 0, mouseY = 0;
	// 方格宽度 边界宽度 棋子半径 十字准星长度（半） 鼠标框长度（半） 鼠标框开口长
	int boxWidth = 40, border = 40, chessR = 24, crossLen = 5, mouseWidth = 16, mouseGap = 8;

	void setup();

	void paintEvent(QPaintEvent*) override;
	void paintLine();
	void paintMouse();
	void paintChess();

	void mouseMoveEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
};

#endif // GAMEWINDOW_H
