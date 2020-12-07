#include <algorithm>
#include <random>
#include <QDebug>
#include "Board.h"

static std::random_device rd;
static std::mt19937 gen(rd());

Board::Board()
{
	memset(board, 0, sizeof(board));
}

Board::PlayerInfo Board::ReversePlayer(Board::PlayerInfo p)
{
	if (p == kPlayerWhite)return kPlayerBlack;
	if (p == kPlayerBlack)return kPlayerWhite;
	return Board::kPlayerNone;
}

unsigned Board::GetAvailableOPs() const
{
	return availableOPs;
}

Board::Result Board::Move(Board::OP& op)
{
	if (gameOver || availableOPs == 0)return Board::kGameOver;

	if (op.player == kPlayerNone)
	{
		if (operations.empty())op.player = kPlayerBlack;
		else op.player = (operations.top().player == kPlayerBlack ? kPlayerWhite : kPlayerBlack);
	}

	double val = 0;
	op.result = Check(op, val);
	op.value = val;
	if (op.result == kNull || op.result == kInvalid || op.result == kBan)
		return op.result;

	if (op.result == kWhiteWin || op.result == kBlackWin)gameOver = true;
	auto copy = op;
	operations.push(copy);
	board[op.x][op.y] = op.player;
	availableOPs--;
	return op.result;
}

Board::OP Board::GetLatestOP()
{
	if (operations.empty()) return { Board::kPlayerNone, 0, 0, Board::kNull, 0.0 };
	else return operations.top();
}

void Board::Reset()
{
	std::stack<OP>().swap(operations);
	memset(board, 0, sizeof(board));
	availableOPs = kBoardSize * kBoardSize;
	this->gameOver = false;
}

Board::Result Board::Check(const Board::OP& op, double& value)
{
	if (op.x > kBoardSize - 1 || op.y > kBoardSize)
		return Board::kInvalid;

	if (board[op.x][op.y] != 0)
		return Board::kInvalid;

	if (operations.empty() && op.player != kPlayerBlack)
		return Board::kInvalid;
	//if (!operations.empty() && op.player == operations.top().player)
	//	return Board::kInvalid;

	double val = 0;
	board[op.x][op.y] = op.player;
	auto win = checkWin(op, val);
	board[op.x][op.y] = kPlayerNone;
	value += val;
	//value += evaluateGame(op);
	if (value >= kWin)value = kWin - kLink;

	if (win == kBan
		|| (win == kBlackWin && op.player != kPlayerBlack)
		|| (win == kWhiteWin && op.player != kPlayerWhite)
		)
		value = kLose;
	else if ((win == kBlackWin && op.player == kPlayerBlack)
		|| (win == kWhiteWin && op.player == kPlayerWhite)
		)
		value = kWin;

	return win;
}

#define CHECK_BORDER(x) ((x) >= 0 && (x) < kBoardSize)

Board::Result Board::checkWin(const Board::OP& op, double& value)
{
	Result result = kNull;
	Result chkLong = kNoError; // 长连判定
	int chk4 = 0; // 四四禁手
	int chk3 = 0; // 三三禁手
	double point = 0; // 评分

	auto counter = [&](auto update1, auto update2)mutable
	{
	  int x1, y1, x2, y2, x3, y3, x4, y4;
	  int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
	  bool live1 = false, live2 = false, live3 = false, live4 = false;
	  double tmpScore = 0, score[3] = { 0, 0, 0 };

	  /*
	  	|  o    | x | x |  o     | x | x | x |  o    | x | x | x |  o    | x | x | x |  o   |
	  	| x3y3  |   |   | x1y1   |   |   |   | op.xy |   |   |   | x2y2  |   |   |   | x4y4 |
	  */
	  for (x1 = op.x, y1 = op.y; CHECK_BORDER(x1) && CHECK_BORDER(y1); update1(x1, y1))
		  if (board[x1][y1] == op.player)cnt1++;
		  else break;
	  for (x2 = op.x, y2 = op.y; CHECK_BORDER(x2) && CHECK_BORDER(y2); update2(x2, y2))
		  if (board[x2][y2] == op.player)cnt2++;
		  else break;

	  if (board[x1][y1] == kPlayerNone)live1 = true;
	  if (board[x2][y2] == kPlayerNone)live2 = true;
	  update1(x1, y1);
	  update2(x2, y2);

	  for (x3 = x1, y3 = y1; CHECK_BORDER(x3) && CHECK_BORDER(y3); update1(x3, y3))
		  if (board[x3][y3] == op.player)cnt3++;
		  else break;
	  for (x4 = x2, y4 = y2; CHECK_BORDER(x4) && CHECK_BORDER(y4); update1(x4, y4))
		  if (board[x4][y4] == op.player)cnt4++;
		  else break;

	  if (board[x3][y3] == kPlayerNone)live3 = true;
	  if (board[x4][y4] == kPlayerNone)live4 = true;

	  /* 检查三三禁手 */
	  bool ban3 = false;
	  if (cnt1 + cnt2 - 1 >= 3 && live1 && live2)/* 活三 */
	  {
		  ban3 = true;
		  score[0] = cnt1 + cnt2 - 1;
	  }
	  if (cnt1 + cnt2 - 1 + cnt3 >= 3 && live1 && (live2 || live3)) /* 冲三 */
	  {
		  ban3 = true;
		  score[1] = cnt1 + cnt2 - 1 + cnt3;
	  }
	  if (cnt1 + cnt2 - 1 + cnt4 >= 3 && live2 && (live1 || live4)) /* 冲三 */
	  {
		  ban3 = true;
		  score[2] = cnt1 + cnt2 - 1 + cnt4;
	  }
	  if (op.player == kPlayerBlack)
		  chk3 += ban3;
	  double tmp = std::max(score[1], score[2]) * kThreeBlockBonus;
	  tmp = std::max(tmp, score[0] * kThreeBonus);
	  tmpScore += tmp;

	  /* 检查四四禁手 */
	  bool ban4 = false;
	  score[0] = score[1] = score[2] = 0;
	  if (cnt1 + cnt2 - 1 >= 4) /* 活四 */
	  {
		  ban4 = true;
		  score[0] = cnt1 + cnt2 - 1;
	  }
	  if (cnt1 + cnt2 - 1 + cnt3 >= 4 && live1) /* 冲四 */
	  {
		  ban4 = true;
		  score[1] = cnt1 + cnt2 - 1 + cnt3;
	  }
	  if (cnt1 + cnt2 - 1 + cnt4 >= 4 && live2) /* 冲四 */
	  {
		  ban4 = true;
		  score[2] = cnt1 + cnt2 - 1 + cnt4;
	  }
	  if (op.player == kPlayerBlack)
		  chk4 += ban4;
	  tmp = std::max(score[1], score[2]) * kFourBlockBonus;
	  tmp = std::max(tmp, score[0] * kFourBonus);
	  tmpScore = std::max(tmpScore, tmp);
	  //tmpScore += tmp;

	  if (tmpScore < 3 * kThreeBlockBonus || cnt1 + cnt2 - 1 < 3)
		  tmpScore += kLink * (cnt1 + cnt2 - 1);

	  /* 检查输赢 */
	  Board::Result tmpResult = kNull;
	  if (cnt1 + cnt2 - 1 > 5) /* 长连禁手判断 */
		  tmpResult = (op.player == kPlayerWhite) ? kWhiteWin : kBan;
	  if (cnt1 + cnt2 - 1 == 5)
	  {
		  if (op.player == kPlayerWhite)
			  tmpResult = kWhiteWin;
		  else
		  {
			  tmpResult = kBlackWin;
			  chkLong = kBlackWin;
		  }
	  }
	  if (result != kWhiteWin && result != kBlackWin && result != kBan)result = tmpResult;

	  /* 评分 */
	  point += tmpScore;
	};

	// check vertical(|)
	auto vu1 = [](int& x, int& y)mutable
	{ y--; };
	auto vu2 = [](int& x, int& y)mutable
	{ y++; };
	counter(vu1, vu2);

	// check horizontal(-)
	auto hu1 = [](int& x, int& y)mutable
	{ x--; };
	auto hu2 = [](int& x, int& y)mutable
	{ x++; };
	counter(hu1, hu2);

	// check slash(/)
	auto su1 = [](int& x, int& y)mutable
	{
	  x--;
	  y--;
	};
	auto su2 = [](int& x, int& y)mutable
	{
	  x++;
	  y++;
	};
	counter(su1, su2);

	// check backslash(\)
	auto bu1 = [](int& x, int& y)mutable
	{
	  x++;
	  y--;
	};
	auto bu2 = [](int& x, int& y)mutable
	{
	  x--;
	  y++;
	};
	counter(bu1, bu2);

	if (result == kWhiteWin)
	{
		value = kWin;
		return kWhiteWin;
	}

	if ((chk3 >= 2 || chk4 >= 2 || result == kBan) && result != kBlackWin)
	{
		value = kLose;
		return kBan;
	}

	if (chkLong == kBlackWin)
	{
		value = kWin;
		return kBlackWin;
	}
	else
	{
		value = point;
		return kNoError;
	}
}

std::vector<Board::OP> Board::GetAvailableOPs(Board::PlayerInfo player)
{
	std::vector<Board::OP> ops;

	if (gameOver)return ops;

	for (int i = 0; i < kBoardSize; i++)
	{
		for (int j = 0; j < kBoardSize; j++)
		{
			if (board[i][j] != kPlayerNone)continue;

			bool found = false;
			for (int k = std::max(0, i - 1); !found && k < kBoardSize && k <= i + 1; k++)
			{
				for (int l = std::max(0, j - 1); !found && l < kBoardSize && l <= j + 1; l++)
				{
					if (board[k][l] != kPlayerNone)
						found = true;
				}
			}
			if (!found)continue;

			Board::OP op(player, i, j, Board::kNull, 0.0);
			double val = 0.0;
			auto result = Check(op, val);
			op.value = val;
			op.result = result;
			if (op.result == kNull || op.result == kInvalid || op.result == kBan)continue;
			op.result = result;
			ops.push_back(op);
		}
	}

	std::shuffle(ops.begin(), ops.end(), gen);
	std::sort(ops.begin(), ops.end());
	return ops;
}

Board::OP Board::Revoke()
{
	Board::OP op;
	if (operations.empty())
	{
		op = { Board::kPlayerNone, 0, 0, Board::kNull, 0.0 };
	}
	else
	{
		op = operations.top();
		operations.pop();
		this->board[op.x][op.y] = Board::kPlayerNone;
		availableOPs++;
		this->gameOver = false;
	}
	return op;
}
