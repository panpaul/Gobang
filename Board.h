/**
 * @file Board.h
 * @brief 棋盘模型定义
 * @author Paul
 */

#ifndef GOBANG__BOARD_H_
#define GOBANG__BOARD_H_

#include <stack>
#include <vector>
#include <cstdint>

class Board
{
 public:
	Board();

 public:
	enum PlayerInfo /// 玩家执棋信息
	{
		kPlayerNone = 0, ///< 占位符
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
		kGameOver = 6, ///< 游戏已经结束
	};

	struct OP /// 每一步操作的定义
	{
		PlayerInfo player = kPlayerBlack; ///< 操作的玩家信息
		uint8_t x = 0, y = 0;             ///< 下棋的位置
		Result result = kNull;            ///< 该步操作的结果
		double value = 0.0;               ///< 该步操作的评分

		OP(PlayerInfo player, uint8_t x, uint8_t y, Result result, double value)
			: player(player), x(x), y(y), result(result), value(value)
		{
		};

		OP() : player(kPlayerNone), x(0), y(0), result(kNull), value(0.0)
		{
		};

		bool operator<(const OP& a) const
		{ // 降序排列
			return value > a.value;
		}

		bool operator==(const OP& a) const
		{
			return x == a.x && y == a.y && player == a.player;
		}
	};

 private:
	static const uint8_t kBoardSize = 15;
	std::stack<OP> operations;
	bool gameOver = false;
	unsigned availableOPs = kBoardSize * kBoardSize;

	const double kWin = 750.0;       // 胜利分值
	const double kFourBonus = 80.0;  // 四连奖励
	const double kThreeBonus = 50.0; // 三连奖励
	const double kLink = 1.0;        // 连子系数
	const double kLose = 0.0;     // 失败分值
	const double kFine = 1.0;        // 负分惩罚
	const double kBlockBonus = 2.5;  // 封锁对方走步奖励

 public:
	int board[kBoardSize][kBoardSize]{}; ///< 棋盘

	Result Move(OP&);         ///< 下一步棋
	Result Check(const OP&, double&); ///< 检查当前落子合法性
	OP Revoke();             ///< 撤回一步操作

	OP GetLatestOP(); ///< 获取最后一步
	std::vector<OP> GetAvailableOPs(PlayerInfo player); ///< 过去所有可行的下法(在已下节点周围1格子范围内)
	void Reset(); ///< 重置

	static PlayerInfo ReversePlayer(PlayerInfo);

 private:
	// TODO 重写
	Result checkWin(const OP& op, double&);
	double evaluateGame(const OP& op);
	// TODO 以下四个函数合并重写
	double evaluateVert_(int x, int y);
	double evaluateHor_(int x, int y);
	double evaluateSla_(int x, int y);
	double evaluateBs_(int x, int y);

};

#endif //GOBANG__BOARD_H_
