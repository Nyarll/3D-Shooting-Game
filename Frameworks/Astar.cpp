#include "AStar.hpp"
#include <algorithm>

using namespace std::placeholders;

bool AStar::Vec2int::operator == (const Vec2int& coordinates_)
{
	return (x == coordinates_.x && y == coordinates_.y);
}

AStar::Vec2int operator + (const AStar::Vec2int& left_, const AStar::Vec2int& right_)
{
	return{ left_.x + right_.x, left_.y + right_.y };
}

AStar::Node::Node(Vec2int coordinates_, Node *parent_)
{
	parent = parent_;
	coordinates = coordinates_;
	G = H = 0;
}

AStar::uint AStar::Node::GetScore()
{
	return G + H;
}

AStar::Generator::Generator()
{
	SetDiagonalMovement(false);
	SetHeuristic(&Heuristic::manhattan);
	m_direction = {
		{ 0, 1 },{ 1, 0 },{ 0, -1 },{ -1, 0 },
		{ -1, -1 },{ 1, 1 },{ -1, 1 },{ 1, -1 }
	};
}

void AStar::Generator::SetWorldSize(Vec2int worldSize_)
{
	m_worldSize = worldSize_;
}

void AStar::Generator::SetDiagonalMovement(bool enable_)
{
	m_directions = (enable_ ? 8 : 4);
}

void AStar::Generator::SetHeuristic(HeuristicFunction heuristic_)
{
	m_heuristic = std::bind(heuristic_, _1, _2);
}

void AStar::Generator::AddCollision(Vec2int coordinates_)
{
	m_walls.push_back(coordinates_);
}

void AStar::Generator::RemoveCollision(Vec2int coordinates_)
{
	auto it = std::find(m_walls.begin(), m_walls.end(), coordinates_);
	if (it != m_walls.end()) 
	{
		m_walls.erase(it);
	}
}

void AStar::Generator::ClearCollisions()
{
	m_walls.clear();
}

AStar::CoordinateList& AStar::Generator::FindPath(Vec2int source_, Vec2int target_)
{
	Node *current = nullptr;
	NodeSet openSet, closedSet;
	openSet.insert(new Node(source_));

	while (!openSet.empty()) 
	{
		current = *openSet.begin();
		for (auto node : openSet) 
		{
			if (node->GetScore() <= current->GetScore()) 
			{
				current = node;
			}
		}

		if (current->coordinates == target_) 
		{
			break;
		}

		closedSet.insert(current);
		openSet.erase(std::find(openSet.begin(), openSet.end(), current));

		for (uint i = 0; i < m_directions; ++i)
		{
			Vec2int newCoordinates(current->coordinates + m_direction[i]);
			if (DetectCollision(newCoordinates) ||
				FindNodeOnList(closedSet, newCoordinates)) 
			{
				continue;
			}

			uint totalCost = current->G + ((i < 4) ? 10 : 14);

			Node *successor = FindNodeOnList(openSet, newCoordinates);
			if (successor == nullptr) 
			{
				successor = new Node(newCoordinates, current);
				successor->G = totalCost;
				successor->H = m_heuristic(successor->coordinates, target_);
				openSet.insert(successor);
			}
			else if (totalCost < successor->G) 
			{
				successor->parent = current;
				successor->G = totalCost;
			}
		}
	}

	CoordinateList path;
	while (current != nullptr) 
	{
		path.push_back(current->coordinates);
		current = current->parent;
	}

	ReleaseNodes(openSet);
	ReleaseNodes(closedSet);

	return path;
}


AStar::Node* AStar::Generator::FindNodeOnList(NodeSet& nodes_, Vec2int coordinates_)
{
	for (auto node : nodes_)
	{
		if (node->coordinates == coordinates_) 
		{
			return node;
		}
	}
	return nullptr;
}

void AStar::Generator::ReleaseNodes(NodeSet& nodes_)
{
	for (auto it = nodes_.begin(); it != nodes_.end();) 
	{
		delete *it;
		it = nodes_.erase(it);
	}
}

bool AStar::Generator::DetectCollision(Vec2int coordinates_)
{
	if (coordinates_.x < 0 || coordinates_.x >= m_worldSize.x ||
		coordinates_.y < 0 || coordinates_.y >= m_worldSize.y ||
		std::find(m_walls.begin(), m_walls.end(), coordinates_) != m_walls.end()) 
	{
		return true;
	}
	return false;
}

AStar::Vec2int AStar::Heuristic::getDelta(Vec2int source_, Vec2int target_)
{
	return{ abs(source_.x - target_.x),  abs(source_.y - target_.y) };
}

AStar::uint AStar::Heuristic::manhattan(Vec2int source_, Vec2int target_)
{
	auto delta = std::move(getDelta(source_, target_));
	return static_cast<uint>(10 * (delta.x + delta.y));
}

AStar::uint AStar::Heuristic::euclidean(Vec2int source_, Vec2int target_)
{
	auto delta = std::move(getDelta(source_, target_));
	return static_cast<uint>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
}

AStar::uint AStar::Heuristic::octagonal(Vec2int source_, Vec2int target_)
{
	auto delta = std::move(getDelta(source_, target_));
	return (10 * (delta.x + delta.y) + (-6) * std::min(delta.x, delta.y));
}