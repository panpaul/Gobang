#include "Game.h"

Game::Game(QWidget* window)
{
	this->board = std::make_shared<Board>();
	this->player = Board::kPlayerBlack;
	mainWindow = window;
}

void Game::Invoke(Board::OP op)
{
	Board::Result r = this->board->Move(op);
	switch (r)
	{
	case Board::kNoError:
		if (this->board->GetLatestOP().player == player)
		{
			// 该AI走子了

			this->engine = new Engine(this->board, Board::ReversePlayer(player));
			auto search = this->engine->Search();
			Invoke(search);
			delete engine;
		}
		break;
	case Board::kNull:
		QMessageBox::critical(mainWindow,
			QMessageBox::tr("Internal Error"),
			QMessageBox::tr("operation result is null"));
		break;
	case Board::kBlackWin:
		QMessageBox::information(mainWindow,
			QMessageBox::tr("Game Over"),
			QMessageBox::tr("Black Wins"));
		break;
	case Board::kWhiteWin:
		QMessageBox::information(mainWindow,
			QMessageBox::tr("Game Over"),
			QMessageBox::tr("White Wins"));
		break;
	case Board::kBan:
		QMessageBox::warning(mainWindow,
			QMessageBox::tr("Warning"),
			QMessageBox::tr("operation is banned"));
		break;
	case Board::kInvalid:
		QMessageBox::warning(mainWindow,
			QMessageBox::tr("Error"),
			QMessageBox::tr("invalid operation"));
		break;
	case Board::kGameOver:
		QMessageBox::information(mainWindow,
			QMessageBox::tr("Game Over"),
			QMessageBox::tr("the game has been over"));
		this->board->Reset();
		break;
	}
}
