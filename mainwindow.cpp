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

void MainWindow::on_playButton_clicked()
{
	this->close();
	auto* gw = new GameWindow();
	gw->show();
}
