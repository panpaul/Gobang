#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	this->game = std::make_shared<Game>(this);
	this->board = game->board;

	ui->setupUi(this);
	this->setFixedSize(border * 2 + boxWidth * 14, border * 2 + boxWidth * 14);
	this->setMouseTracking(true);
	this->centralWidget()->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	/* 画格子 */
	painter.setPen(QPen(Qt::darkGreen, 2, Qt::SolidLine));
	for (int i = 0; i < 15; i++)
	{
		// 画横线
		painter.drawLine(border, border + boxWidth * i, border + boxWidth * 14, border + boxWidth * i);
		// 画竖线
		painter.drawLine(border + boxWidth * i, border, border + boxWidth * i, boxWidth + boxWidth * 14);
	}

	/* 画棋子 */
	painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (board->board[i][j] == Board::kPlayerWhite)
			{
				painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
				painter.drawEllipse(border - chessR / 2 + boxWidth * i,
					border - chessR / 2 + boxWidth * j,
					chessR, chessR);
			}
			else if (board->board[i][j] == Board::kPlayerBlack)
			{
				painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
				painter.drawEllipse(border - chessR / 2 + boxWidth * i,
					border - chessR / 2 + boxWidth * j,
					chessR, chessR);
			}
		}
	}

	/* 最新一次落子十字点 */
	painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
	Board::OP op = board->GetLatestOP();
	if (op.player != Board::kPlayerNone)
	{
		painter.drawLine(border - crossLen + op.x * boxWidth, border + op.y * boxWidth,
			border + crossLen + op.x * boxWidth, border + op.y * boxWidth);
		painter.drawLine(border + op.x * boxWidth, border - crossLen + op.y * boxWidth,
			border + op.x * boxWidth, border + crossLen + op.y * boxWidth);
	}

	/* 画鼠标框 */
	painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
	if ((mouseX > border - mouseWidth && mouseX < border + boxWidth * 14 + mouseWidth)
		&& (mouseY > border - mouseWidth && mouseY < border + boxWidth * 14 + mouseWidth)
		)
	{
		painter.drawLine(mouseX - mouseWidth, mouseY - mouseWidth, mouseX - mouseGap, mouseY - mouseWidth);
		painter.drawLine(mouseX - mouseWidth, mouseY - mouseWidth, mouseX - mouseWidth, mouseY - mouseGap);

		painter.drawLine(mouseX - mouseWidth, mouseY + mouseWidth, mouseX - mouseGap, mouseY + mouseWidth);
		painter.drawLine(mouseX - mouseWidth, mouseY + mouseWidth, mouseX - mouseWidth, mouseY + mouseGap);

		painter.drawLine(mouseX + mouseWidth, mouseY + mouseWidth, mouseX + mouseGap, mouseY + mouseWidth);
		painter.drawLine(mouseX + mouseWidth, mouseY + mouseWidth, mouseX + mouseWidth, mouseY + mouseGap);

		painter.drawLine(mouseX + mouseWidth, mouseY - mouseWidth, mouseX + mouseGap, mouseY - mouseWidth);
		painter.drawLine(mouseX + mouseWidth, mouseY - mouseWidth, mouseX + mouseWidth, mouseY - mouseGap);
	}

}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
	mouseX = event->x();
	mouseY = event->y();

	this->update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	int x = event->x();
	int y = event->y();

	x = (x - border + boxWidth / 2) / boxWidth;
	y = (y - border + boxWidth / 2) / boxWidth;

	int minX, minY, maxX, maxY;
	minX = border - mouseWidth + x * boxWidth;
	maxX = border + mouseWidth + x * boxWidth;
	minY = border - mouseWidth + y * boxWidth;
	maxY = border + mouseWidth + y * boxWidth;

	if (mouseX > minX && mouseX < maxX && mouseY > minY && mouseY < maxY)
	{
		qDebug() << "New Move (x,y) = (" << x << "," << y << ")";

		Board::OP op(Board::kPlayerNone, x, y, Board::kNull);
		this->game->Invoke(op);

		this->update();
	}

}
