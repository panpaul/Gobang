#include "Game.h"

Game::Game(QWidget* window)
{
	this->board = std::make_shared<Board>();
	this->player = Board::kPlayerBlack;
	this->version = kGameHumanComputer;
	mainWindow = window;
}

Game::Game(QWidget* window, GameType version)
{
	this->board = std::make_shared<Board>();
	this->version = version;
	switch (version)
	{
	case kGameHumanComputer:
	case kGameHH:
		player = Board::kPlayerBlack;
		break;
	case kGameComputerHuman:
	case kGameCC:
		player = Board::kPlayerWhite;
		Board::OP op = { Board::kPlayerBlack, 7, 7, Board::kNull, 0 };
		Invoke(op);
		break;
	}
	mainWindow = window;
}

void Game::Invoke(Board::OP op)
{
	Board::Result r = this->board->Move(op);
	switch (r)
	{
	case Board::kNoError:
		if (version == kGameCC)player = Board::ReversePlayer(player);
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
		secondEnd = true;
		break;
	case Board::kWhiteWin:
		QMessageBox::information(mainWindow,
			QMessageBox::tr("Game Over"),
			QMessageBox::tr("White Wins"));
		secondEnd = true;
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
		if (secondEnd)
			Reset();
		else
			secondEnd = true;
		break;
	}
}

void Game::Next()
{
	if (version == kGameHH)return;
	if (secondEnd)return;
	if (this->board->GetLatestOP().player == player)
	{
		// 该AI走子了
		this->engine = new Engine(this->board, Board::ReversePlayer(player));
		auto search = this->engine->Search();
		Invoke(search);
		delete engine;
	}
}

void Game::Reset()
{
	secondEnd = false;
	this->board->Reset();
	if (version == kGameCC || version == kGameComputerHuman)
	{
		player = Board::kPlayerWhite;
		Board::OP op = { Board::kPlayerBlack, 7, 7, Board::kNull, 0 };
		Invoke(op);
	}
}
