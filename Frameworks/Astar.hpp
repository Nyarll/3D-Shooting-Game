#pragma once

// A star

#ifndef ASTAR_DEFINES
#define ASTAR_DEFINES

#include <vector>
#include <functional>
#include <set>

namespace AStar
{
	struct Vec2int
	{
		int x, y;

		bool operator == (const Vec2int& coordinates_);
	};

	using uint = unsigned int;
	using HeuristicFunction = std::function<uint(Vec2int, Vec2int)>;
	using CoordinateList = std::vector<Vec2int>;

	struct Node
	{
		uint G, H;
		Vec2int coordinates;
		Node *parent;

		Node(Vec2int coord_, Node *parent_ = nullptr);
		uint GetScore();
	};

	using NodeSet = std::set<Node*>;

	class Generator
	{
		bool DetectCollision(Vec2int coordinates_);
		Node* FindNodeOnList(NodeSet& nodes_, Vec2int coordinates_);
		void ReleaseNodes(NodeSet& nodes_);

	public:
		Generator();

		// <ワールドサイズ設定>
		void SetWorldSize(Vec2int worldSize_);
		// <斜め移動するかどうか>
		void SetDiagonalMovement(bool enable_);
		// <ヒューリスティックの設定>
		void SetHeuristic(HeuristicFunction heuristic_);
		// <source_ から target_ までの最短ルート>
		CoordinateList& FindPath(Vec2int source_, Vec2int target_);

		// <壁の登録>
		void AddCollision(Vec2int coordinates_);
		// <壁の忘却>
		void RemoveCollision(Vec2int coordinates_);
		// <壁全消去>
		void ClearCollisions();

	private:
		HeuristicFunction m_heuristic;
		CoordinateList m_direction, m_walls;
		Vec2int m_worldSize;
		uint m_directions;
	};

	class Heuristic
	{
		static Vec2int getDelta(Vec2int source_, Vec2int target_);

	public:
		static uint manhattan(Vec2int source_, Vec2int target_);
		static uint euclidean(Vec2int source_, Vec2int target_);
		static uint octagonal(Vec2int source_, Vec2int target_);
	};
}

#endif