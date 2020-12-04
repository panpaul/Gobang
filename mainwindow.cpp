#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "gamewindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_playHC_clicked()
{
	this->close();
	auto* gw = new GameWindow(nullptr, Game::kGameHumanComputer);
	gw->show();
}

void MainWindow::on_playCH_clicked()
{
	this->close();
	auto* gw = new GameWindow(nullptr, Game::kGameComputerHuman);
	gw->show();
}

void MainWindow::on_playCC_clicked()
{
	this->close();
	auto* gw = new GameWindow(nullptr, Game::kGameCC);
	gw->show();
}

void MainWindow::on_playHH_clicked()
{
	this->close();
	auto* gw = new GameWindow(nullptr, Game::kGameHH);
	gw->show();
}
