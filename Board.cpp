#include "Board.h"

Board::Board()
{
	memset(board, 0, sizeof(board));

}

Board::Result Board::Move(Board::OP op)
{
	op.result = Check(op);
	if (op.result == kNull || op.result == kInvalid || op.result == kBan)
		return op.result;
	operations.push(op);
	return op.result;
}

Board::Result Board::Check(const Board::OP& op)
{
	if (op.x > kBoardSize - 1 || op.y > kBoardSize)
		return Board::kInvalid;

	if (board[op.x][op.y] != 0)
		return Board::kInvalid;

	auto win = checkWin(op);
	if (win != kNoError)
		return win;

	return Board::kNoError;
}

#define CHECK_BORDER(x) ((x) >= 0 && (x) < kBoardSize)
#define CHECK_WIN() \
    if (cnt1 + cnt2 + 1 > 5) \
        return (op.player == kPlayerWhite) ? kWhiteWin : kBan; \
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

Board::Result Board::checkWin(const Board::OP& op)
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
		// 四四禁手
		CHECK_44(op.x, j1, op.x, j2)
		// 三三禁手
		CHECK_33(op.x, j1, op.x, j2, op.x, j3, op.x, j4)
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
		// 四四禁手
		CHECK_44(i1, op.y, i2, op.y)
		// 三三禁手
		CHECK_33(i1, op.y, i2, op.y, i3, op.y, i4, op.y)

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
		// 四四禁手
		CHECK_44(i1, j1, i2, j2)
		// 三三禁手
		CHECK_33(i1, j1, i2, j2, i3, j3, i4, j4)
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
		// 四四禁手
		CHECK_44(i1, j1, i2, j2)
		// 三三禁手
		CHECK_33(i1, j1, i2, j2, i3, j3, i4, j4)
	}

	if (chk3 >= 2 || chk4 >= 2)return Board::Result::kBan;

	return chkLong;
}

