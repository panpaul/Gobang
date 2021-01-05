/**
 * @file Engine.h
 * @brief 游戏引擎
 * @author Paul
 */

#ifndef GOBANG__ENGINE_H_
#define GOBANG__ENGINE_H_

#include "Board.h"

#include <cmath>
#include <memory>
#include <vector>

class Engine
{
 public:
	struct Node
	{
		Board::OP op; ///< 当前节点的操作

		bool fullExpanded;  ///< 是否被完全扩展
		unsigned numVisits; ///< 访问的次数
		double reward;      ///< 当前节点价值

		Node* parent;              ///< 父节点
		std::vector<Node*> child;  ///< 孩子节点

		Node()
		{
			op = { Board::kPlayerNone, 0, 0, Board::kNull, 0.0 };
			fullExpanded = false;
			numVisits = 0;
			reward = 0;
			parent = nullptr;
		}
	};

 private:
	std::shared_ptr<Board> board;
	Node* Root;

	static constexpr double UCB_C = 1.414;  ///< UCB公式中的常数
	const int ExecuteTimes = 7500;          ///< 搜索执行次数
	const int SimulationMaxDepth = 40;      ///< 模拟时模拟深度
	int PopTimes = 0;                       ///< 复原需要的撤回次数

	double normalizeMin, normalizeMax;

 public:
	explicit Engine(std::shared_ptr<Board>);
	~Engine();
	Board::OP Search();     ///< 执行搜索
	bool MCTS = false;      ///< 当前落子决策是否采用了蒙特卡洛数搜索算法

 private:
	/**
	 * @brief 计算某个节点的ucb值
	 * @param 带计算的节点
	 * @return 该节点的ucb值
	 */
	static double ucb(Node* node, double nMin, double nMax);

	/**
	 * @brief 循环执行的子结构
	 */
	void Execute();

	/**
	 * @brief 选择节点
	 * @details 选出的节点不在栈中
	 * @return 选出的节点
	 */
	Node* SelectNode(Node*);

	/**
	 * @brief 扩展节点
	 * @details 扩展可以访问的节点
	 * @param 带扩展节点且该节点未被加入栈中
	 * @return 扩展后的输入节点
	 */
	Node* Expand(Node*);

	/**
	 * @brief 模拟走子
	 * @return 模拟走子的结果
	 */
	double Simulation(Node*);

	/**
	 * @brief 结果广播
	 * @brief 采用平均值
	 * @param 叶子节点
	 * @param 要广播的值
	 */
	static void BackPropagate(Node*, double);

	/**
	 * @brief 获取给定节点下最佳的孩子节点
	 * @param 给定的节点
	 * @return 最佳子节点
	 */
	Node* GetBestChild(Node*);

	/**
	 * @brief 随机选取一个孩子节点
	 * @param 给定的节点
	 * @return 随机选的子节点
	 */
	static Node* GetRandomChild(Node*);

	/**
	 * @brief 对孩子节点的reward进行归一化
	 * @param 给定的节点
	 */
	void Normalize(Node*);

	/**
	 * @brief 回收内存
	 */
	void Destroy(Node*&);

	struct SortStruct
	{
		// 解决sort必须要static的毛病
		Engine* m;

		explicit SortStruct(Engine* p) : m(p)
		{
		};

		bool operator()(Node* i, Node* j) const
		{
			return ucb(i, m->normalizeMin, m->normalizeMax) < ucb(j, m->normalizeMin, m->normalizeMax);
		}
	};
};

#endif //GOBANG__ENGINE_H_
