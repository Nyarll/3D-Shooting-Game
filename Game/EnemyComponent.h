#pragma once

#include "../Frameworks/Component.h"
#include "../Frameworks/Astar.hpp"

class EnemyComponent : public Component
{
private:
	enum Direction
	{
		Left,
		Right,
		Up,
		Down
	}m_direction;


	DirectX::SimpleMath::Vector2 m_gridOldPos;
	DirectX::SimpleMath::Vector2 m_gridPosition;
	DirectX::SimpleMath::Vector2 m_v;

	bool isMove = false;
	int m_cnt = 0;

	std::unique_ptr<DirectX::GeometricPrimitive>	m_geo;

	DirectX::SimpleMath::Vector2 m_dir;
	float m_angle = 0;

	std::vector<DirectX::SimpleMath::Vector2>	m_route;

	const int MOVE_DIV = 16;
	std::string jsonFileName = "";

public:

	bool Move(GameContext& context);

	void SetGridPosition(GameContext& context, DirectX::SimpleMath::Vector2 gridPos);

	DirectX::SimpleMath::Vector2 GetGridPosition()
	{
		return m_gridPosition;
	}

	void SetJsonFileName(std::string fileName)
	{
		jsonFileName = fileName;
	}

	void Initialize(GameContext& context)override;
	void Update(GameContext& context)override;
	void Render(GameContext& context)override;

private:
	int GetMovingDirection(GameContext& context);
};