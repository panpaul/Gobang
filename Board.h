/**
 * @file Board.h
 * @brief 棋盘模型定义
 * @author Paul
 */

#ifndef GOBANG__BOARD_H_
#define GOBANG__BOARD_H_

#include <cstdint>
#include <cstring>
#include <stack>

class Board
{
 public:
	Board();

 public:
	enum PlayerInfo /// 玩家执棋信息
	{
		kPlayerBlack = 1, ///< 当前黑方执棋
		kPlayerWhite = 2, ///< 当前白方执棋
	};

	enum Result /// 每一步操作的结果
	{
		kNull = 0,     ///< 尚未计算结果
		kBlackWin = 1, ///< 黑方胜利
		kWhiteWin = 2, ///< 白方胜利
		kNoError = 3,  ///< 当前操作无异常
		kBan = 4,      ///< 黑方触发禁手规则
		kInvalid = 5,  ///< 无效的操作
	};

	struct OP /// 每一步操作的定义
	{
		PlayerInfo player = kPlayerBlack; ///< 操作的玩家信息
		uint8_t x = 0, y = 0;             ///< 下棋的位置
		Result result = kNull;            ///< 该步操作的结果
	};

 private:
	static const uint8_t kBoardSize = 15;
	std::stack<OP> operations;

 public:
	int board[kBoardSize][kBoardSize]{};
	Result Move(OP op);
	Result Check(const OP& op);

 private:
	Result checkWin(const OP& op);
};

#endif //GOBANG__BOARD_H_
