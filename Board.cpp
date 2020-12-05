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
	auto win = checkWin(op, val);
	value += val;
	value += evaluateGame(op);
	if (value >= 740)value = 740;

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
#define CHECK_WIN() \
    if (cnt1 + cnt2 + 1 > 5) \
    { \
        return (op.player == kPlayerWhite) ? kWhiteWin : kBan; \
    } \
    if (cnt1 + cnt2 + 1 == 5) \
    { \
        if (op.player == kPlayerWhite) \
        { return kWhiteWin; } \
        else \
        { chkLong = kBlackWin; } \
    }
#define CHECK_44(x1, y1, x2, y2) \
    if ((cnt1 + cnt2 + 1 == 4 && (board[(x1)][(y1)] == 0 || board[(x2)][(y2)] == 0)) /* 活四 */ \
        || (cnt1 + cnt2 + 1 + cnt3 >= 4 && board[(x1)][(y1)] == 0) /* 冲四 */ \
        || (cnt1 + cnt2 + 1 + cnt4 >= 4 && board[(x2)][(y2)] == 0)) /* 冲四 */ \
            chk4++;
#define CHECK_33(x1, y1, x2, y2, x3, y3, x4, y4) \
    bool ban3 = false; \
    if ((cnt1 + cnt2 + 1 + cnt3 >= 3 && board[(x1)][(y1)] == 0)) /* 冲三 */ \
        if (board[(x2)][(y2)] == 0 || board[(x3)][(y3)] == 0) \
            ban3 = true; \
    if ((cnt1 + cnt2 + 1 + cnt4 >= 3 && board[(x2)][(y2)] == 0)) /* 冲三 */ \
        if (board[(x1)][(y1)] == 0 || board[(x4)][(y4)] == 0) \
            ban3 = true; \
    if (cnt1 + cnt2 + 1 == 3 && board[(x1)][(y1)] == 0 && board[(x1)][(y2)] == 0) \
        ban3 = true; \
    chk3 += ban3;

Board::Result Board::checkWin(const Board::OP& op, double& value)
{
	Board::Result chkLong = kNoError; // 长连判定
	int chk4 = 0; // 四四禁手
	int chk3 = 0; // 三三禁手

	// check vertical(|)
	{
		int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
		int j1, j2, j3, j4;

		for (j1 = op.y - 1; CHECK_BORDER(j1); j1--)
			if (board[op.x][j1] == op.player)cnt1++;
			else break;
		for (j2 = op.y + 1; CHECK_BORDER(j2); j2++)
			if (board[op.x][j2] == op.player)cnt2++;
			else break;

		for (j3 = j1 - 1; CHECK_BORDER(j3); j3--)
			if (board[op.x][j3] == op.player)cnt3++;
			else break;
		for (j4 = j2 + 1; CHECK_BORDER(j4); j4++)
			if (board[op.x][j4] == op.player)cnt4++;
			else break;

		// 胜利判定
		CHECK_WIN()
		if (op.player == kPlayerBlack)
		{
			// 四四禁手
			CHECK_44(op.x, j1, op.x, j2)
			// 三三禁手
			CHECK_33(op.x, j1, op.x, j2, op.x, j3, op.x, j4)
		}
	}


	// check horizontal(-)
	{
		int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
		int i1, i2, i3, i4;

		for (i1 = op.x - 1; CHECK_BORDER(i1); i1--)
			if (board[i1][op.y] == op.player)cnt1++;
			else break;
		for (i2 = op.x + 1; CHECK_BORDER(i2); i2++)
			if (board[i2][op.y] == op.player)cnt2++;
			else break;
		for (i3 = i1 - 1; CHECK_BORDER(i3); i3--)
			if (board[i3][op.y] == op.player)cnt3++;
			else break;
		for (i4 = i2 + 1; CHECK_BORDER(i4); i4++)
			if (board[i4][op.y] == op.player)cnt4++;
			else break;

		// 胜利判定
		CHECK_WIN()
		if (op.player == kPlayerBlack)
		{
			// 四四禁手
			CHECK_44(i1, op.y, i2, op.y)
			// 三三禁手
			CHECK_33(i1, op.y, i2, op.y, i3, op.y, i4, op.y)
		}

	}

	// check slash(/)
	{
		int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
		int i1, j1, i2, j2, i3, j3, i4, j4;
		for (i1 = op.x - 1, j1 = op.y - 1; CHECK_BORDER(i1) && CHECK_BORDER(j1); i1--, j1--)
			if (board[i1][j1] == op.player)cnt1++;
			else break;
		for (i2 = op.x + 1, j2 = op.y + 1; CHECK_BORDER(i2) && CHECK_BORDER(j2); i2++, j2++)
			if (board[i2][j2] == op.player)cnt2++;
			else break;
		for (i3 = i1 - 1, j3 = j1 - 1; CHECK_BORDER(i3) && CHECK_BORDER(j3); i3--, j3--)
			if (board[i3][j3] == op.player)cnt3++;
			else break;
		for (i4 = i2 + 1, j4 = j2 + 1; CHECK_BORDER(i4) && CHECK_BORDER(j4); i4++, j4++)
			if (board[i4][j4] == op.player)cnt4++;
			else break;

		// 胜利判定
		CHECK_WIN()
		if (op.player == kPlayerBlack)
		{
			// 四四禁手
			CHECK_44(i1, j1, i2, j2)
			// 三三禁手
			CHECK_33(i1, j1, i2, j2, i3, j3, i4, j4)
		}
	}

	// check backslash(\)
	{
		int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
		int i1, j1, i2, j2, i3, j3, i4, j4;
		for (i1 = op.x - 1, j1 = op.y + 1; CHECK_BORDER(i1) && CHECK_BORDER(j1); i1--, j1++)
			if (board[i1][j1] == op.player)cnt1++;
			else break;
		for (i2 = op.x + 1, j2 = op.y - 1; CHECK_BORDER(i2) && CHECK_BORDER(j2); i2++, j2--)
			if (board[i2][j2] == op.player)cnt2++;
			else break;
		for (i3 = i1 - 1, j3 = j1 + 1; CHECK_BORDER(i3) && CHECK_BORDER(j3); i3--, j3++)
			if (board[i3][j3] == op.player)cnt3++;
			else break;
		for (i4 = i2 + 1, j4 = j2 - 1; CHECK_BORDER(i4) && CHECK_BORDER(j4); i4++, j4--)
			if (board[i4][j4] == op.player)cnt4++;
			else break;

		// 胜利判定
		CHECK_WIN()
		if (op.player == kPlayerBlack)
		{
			// 四四禁手
			CHECK_44(i1, j1, i2, j2)
			// 三三禁手
			CHECK_33(i1, j1, i2, j2, i3, j3, i4, j4)
		}
	}

	value = chk3 * kThreeBonus + chk4 * kFourBonus;
	if (chk3 >= 2 || chk4 >= 2)return Board::Result::kBan;

	return chkLong;
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

Board::PlayerInfo Board::ReversePlayer(Board::PlayerInfo p)
{
	if (p == kPlayerWhite)return kPlayerBlack;
	if (p == kPlayerBlack)return kPlayerWhite;
	return Board::kPlayerNone;
}

double Board::evaluateGame(const OP& op)
{
	// 时间复杂度+++++

	int backup = board[op.x][op.y];
	board[op.x][op.y] = op.player;

	double points = 0;

	points += evaluateVert_(op.x, op.y);
	points += evaluateHor_(op.x, op.y);
	points += evaluateSla_(op.x, op.y);
	points += evaluateBs_(op.x, op.y);

	board[op.x][op.y] = backup;

	return points;
}

#define eva_head_ double cnt = 0; \
    auto player = static_cast<PlayerInfo>(board[x][y]); \
    int side1x = kBoardSize + 1, side1y = kBoardSize + 1, \
        side2x = kBoardSize + 1, side2y = kBoardSize + 1;

#define eva_tail_ double bonus = 0;\
    if (cnt == 1)\
    {\
        double cnt3 = cnt1 + cnt2;\
        if (side1x != kBoardSize + 1 && side1y != kBoardSize + 1 && \
            side2x != kBoardSize + 1 && side2y != kBoardSize + 1) \
            cnt3--;\
        if (cnt3 <= 0)bonus = 0;\
        else if (cnt3 < 3)bonus = cnt3 * kLink;\
        else if (cnt3 == 3)bonus = 3 * kThreeBonus;\
        else if (cnt3 > 3)bonus = cnt3 * kFourBonus;\
    }\
    bonus *= kBlockBonus;\
    double self = 0;               \
    if (cnt >= 5)self = kWin;\
    if (cnt == 4)self = 4 * kFourBonus;\
    else if (cnt == 3)self = 3 * kThreeBonus;\
    else self = cnt * kLink;\
    return self + bonus;

#define eva_self_(ic, jc, upd1, upd2) \
for (int i = (ic), j = (jc); CHECK_BORDER(i) && CHECK_BORDER(j); (upd1))\
    if (board[i][j] == player)\
        cnt++;\
    else\
    {\
        if (board[i][j] == ReversePlayer(player))\
        {\
            side1x = i;\
            side1y = j;\
            cnt -= 0.5;\
        }\
        break;\
    }\
for (int i = (ic), j = (jc); CHECK_BORDER(i) && CHECK_BORDER(j); (upd2))\
    if (board[i][j] == player)\
        cnt++;\
    else\
    {\
        if (board[i][j] == ReversePlayer(player))\
        {\
            side2x = i;\
            side2y = j;\
            cnt -= 0.5;\
        }\
        break;\
    }\
cnt -= 1;\
int cnt1 = 0, cnt2 = 0;\
if (side1x != kBoardSize + 1 && side1y != kBoardSize + 1)\
{\
    for (int i = side1x, j = side1y; CHECK_BORDER(i) && CHECK_BORDER(j); (upd1))\
        if (board[i][j] == ReversePlayer(player))\
            cnt1++;\
        else break;\
}\
if (side2x != kBoardSize + 1 && side2y != kBoardSize + 1)\
{\
    for (int i = side2x, j = side2y; CHECK_BORDER(i) && CHECK_BORDER(j); (upd2))\
        if (board[i][j] == ReversePlayer(player))\
            cnt2++;\
        else break;\
}

double Board::evaluateVert_(int x, int y)
{
	eva_head_

	eva_self_(x, y, (j++), (j--))

	eva_tail_
}

double Board::evaluateHor_(int x, int y)
{
	eva_head_

	eva_self_(x, y, (i++), (i--))

	eva_tail_

}

double Board::evaluateSla_(int x, int y)
{
	eva_head_

	eva_self_(x, y, (i++, j++), (i--, j--))

	eva_tail_
}

double Board::evaluateBs_(int x, int y)
{
	eva_head_

	eva_self_(x, y, (i++, j--), (i--, j++))

	eva_tail_
}
