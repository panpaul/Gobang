#include "Engine.h"
#include <QDebug>

#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip>

static std::random_device rd;
static std::mt19937 gen(rd());

Engine::Engine(std::shared_ptr<Board> board, Board::PlayerInfo ai)
{
	this->board = std::move(board);
	this->AI = ai;
	this->Root = new Node;
}

double Engine::ucb(Engine::Node* node)
{
	return node->reward + UCB_C * sqrt(log(node->parent->numVisits) / node->numVisits);
}

bool Engine::Node::operator<(Node* node) const
{
	return ucb(const_cast<Node*>(this)) < ucb(node);
}

void debug(Engine::Node* r)
{
	double x[15][15] = {{ 0 }};
	for (auto& y:r->child)
		x[y->op.x][y->op.y] = y->reward;
	for (int j = 0; j < 15; j++)
	{
		for (auto& i : x)
		{
			std::cout << std::setw(5) << std::setfill(' ') << std::setprecision(2) << i[j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

Board::OP Engine::Search()
{
	mcts = false;

	qDebug() << "(" << board->GetLatestOP().x << "," << board->GetLatestOP().y << ") Executed by User";

	// 优先使用特殊判定: 三连、四连
	auto scores = board->GetAvailableOPs(board->GetLatestOP().player);
	auto selfScores = board->GetAvailableOPs(Board::ReversePlayer(board->GetLatestOP().player));

	if (!scores.empty() && !selfScores.empty())
	{
		qDebug() << "\tRules: Self->" << selfScores[0].value << " Enemy->" << scores[0].value;
		auto op = Board::OP{ Board::kPlayerNone, 0, 0, Board::kNull, 0 };
		bool rule = false;
		if (selfScores[0].value >= Board::kWin - 20)
		{
			rule = true;
			op = selfScores[0];
			qDebug() << "(" << op.x << "," << op.y << ") Executed by Rules(Self) with score:" << op.value << "\n";
		}
		else if (scores[0].value >= 400060)
		{
			rule = true;
			op = scores[0];
			qDebug() << "(" << op.x << "," << op.y << ") Executed by Rules(Enemy) with score:" << op.value << "\n";
		}
		else if (selfScores[0].value >= 400060)
		{
			rule = true;
			op = selfScores[0];
			qDebug() << "(" << op.x << "," << op.y << ") Executed by Rules(Self) with score:" << op.value << "\n";
		}

		op.player = board->ReversePlayer(board->GetLatestOP().player);
		op.value = 0;
		op.result = Board::kNull;

		// 防止禁手
		double val = 0;
		auto valid = board->Check(op, val);

		if (rule && valid != Board::kBan && valid != Board::kGameOver && valid != Board::kInvalid)
			return op;
	}

	this->Root->op = board->GetLatestOP();
	this->Root->parent = nullptr;
	this->Root->numVisits = 0;
	this->Root->reward = 0;
	this->Root->fullExpanded = false;

	// 剔除根节点
	board->Revoke();
	PopTimes = 0;

	for (int i = 0; i < ExecuteTimes; i++)
		Execute();

	// 获取最优节点
	Normalize(Root);
	auto* child = GetBestChild(Root);
	assert(child != nullptr);
	auto op = child->op;

	// 还原用户走子
	board->Move(this->Root->op);

	qDebug() << "(" << op.x << "," << op.y << ") Executed by MCTS with score:" << op.value << "\n";
	mcts = true;

	// TODO 性能优化，不一定每次重新计算
	Destroy(Root);

	return op;
}

void Engine::Execute()
{
	auto* node = SelectNode(Root);
	assert(node != nullptr);

	node = Expand(node);
	assert(node != nullptr);

	auto value = Simulation(node);

	BackPropagate(node, value);

	while (PopTimes--)
		board->Revoke();
}

Engine::Node* Engine::SelectNode(Engine::Node* node)
{
	if (node == nullptr)return nullptr;

	while (!node->child.empty())
	{
		// 现场保存
		board->Move(node->op);
		PopTimes++;

		if (!node->fullExpanded)
			for (auto& child: node->child)
				if (child->numVisits == 0)
					return child; // 如果存在没有被模拟过的节点则返回该节点

		node->fullExpanded = true; // 所有子节点都被访问过了
		node = GetRandomChild(node); // 随机选取一个节点开始扩展
	}

	return node;
}

Engine::Node* Engine::Expand(Engine::Node* node)
{
	if (node == nullptr)return nullptr;

	if (node->op.result != Board::kNoError)
		return node; // 当前节点已经结束，直接返回

	// 现场保存
	bool duplicate = (board->Move(node->op) == Board::kInvalid);
	if (!duplicate) // 防止重复下
		PopTimes++;

	// 获取可以下的位置
	auto ops = board->GetAvailableOPs(Board::ReversePlayer(node->op.player));
	// 添加到node的孩子节点
	for (auto& op : ops)
	{
		// 去重
		bool flag = false;
		for (auto& r: node->child)
		{
			if (r->op == op)
			{
				flag = true;
				break;
			}
		}
		if (flag)continue;

		auto* child = new Node;
		child->parent = node;
		child->op = op;
		child->reward = op.value;
		child->fullExpanded = false;
		child->numVisits = 0;

		// 扩展孩子节点
		node->child.push_back(child);
	}

	//node->fullExpanded = true;

	// 现场恢复
	if (!duplicate)
	{
		board->Revoke();
		PopTimes--;
	}

	return node;
}

double Engine::Simulation(Engine::Node* node)
{
	if (node == nullptr)return 0;
	// 现场保存
	board->Move(node->op);
	PopTimes++;

	Node* child = GetRandomChild(node);
	if (child == nullptr)return 0;

	// 开始放飞自我
	auto op = child->op;
	auto player = child->op.player;
	auto ops = board->GetAvailableOPs(Board::ReversePlayer(player));
	double value = 0;
	for (int i = 0; i < SimulationMaxDepth; i++)
	{
		// 保存孩子节点现场
		board->Move(op);
		PopTimes++;

		// 更新
		player = Board::ReversePlayer(player);
		ops = board->GetAvailableOPs(player);
		//value = op.value;

		if (ops.empty())
		{
			player = Board::ReversePlayer(player);
			break;
		}

		op = ops[0];
		value = op.value;

		if (op.result != Board::kNoError)
			break;
	}

	if (node->op.player != player)value = -value;

	return value;
}

void Engine::BackPropagate(Engine::Node* node, double reward)
{
	while (node != nullptr)
	{
		// TODO: 策略调整 平均值? 最小值? 最大值?
		node->reward = (node->reward * node->numVisits + reward);
		node->numVisits++;
		node->reward /= node->numVisits;
		node = node->parent;
		reward = -reward; // 翻转
	}
}

Engine::Node* Engine::GetBestChild(Engine::Node* node)
{
	if (node == nullptr)return nullptr;
	if (node->child.empty())return nullptr;

	std::shuffle(node->child.begin(), node->child.end(), gen);
	std::sort(node->child.begin(), node->child.end());

	//auto best = (node->op.player != this->AI) ? (node->child[0]) : (node->child[node->child.size() - 1]);
	return node->child[0];
}

Engine::Node* Engine::GetRandomChild(Engine::Node* node)
{
	if (node == nullptr)return nullptr;
	if (node->child.empty())return nullptr;

	std::shuffle(node->child.begin(), node->child.end(), gen);
	return node->child[0];
}

void Engine::Destroy(Engine::Node*& node)
{
	if (node == nullptr)return;
	while (!node->child.empty())
	{
		Destroy(node->child.front());
		node->child.erase(node->child.begin());
		//node->child.pop_front();
	}
	node->parent = nullptr;
	delete node;
	node = nullptr;
}

Engine::~Engine()
{
	Destroy(Root);
}

void Engine::Normalize(Engine::Node* node)
{
	double normalizeMin = Board::kWin;
	double normalizeMax = 0;

	for (auto& n:node->child)
	{
		normalizeMax = std::max(n->reward, normalizeMax);
		normalizeMin = std::min(n->reward, normalizeMin);
	}

	if (normalizeMin == normalizeMax)
		for (auto& c : node->child)
			c->reward = 0.5;
	else
		for (auto& c : node->child)
			c->reward = (c->reward - normalizeMin) / (normalizeMax - normalizeMin);

}
