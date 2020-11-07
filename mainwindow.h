#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Game.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
 Q_OBJECT

 public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

 private:
	Ui::MainWindow* ui;
	std::shared_ptr<Game> game;
	std::shared_ptr<Board> board;

 private:
	int mouseX = 0, mouseY = 0;
	// 方格宽度 边界宽度 棋子半径 十字准星长度（半） 鼠标框长度（半） 鼠标框开口长
	int boxWidth = 40, border = 40, chessR = 24, crossLen = 5, mouseWidth = 16, mouseGap = 8;
	void paintEvent(QPaintEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
};

#endif // MAINWINDOW_H
