#pragma once

#include "../Frameworks/Component.h"
#include "../Frameworks/GameContext.h"

class PlayerComponent : public Component
{
private:
	DirectX::SimpleMath::Vector2 m_gridOldPos;
	DirectX::SimpleMath::Vector2 m_gridPosition;
	DirectX::SimpleMath::Vector2 m_v;

	bool isMove = false;
	int m_cnt = 0;

	std::unique_ptr<DirectX::GeometricPrimitive>	m_geo;

	DirectX::SimpleMath::Vector2 m_dir;
	float m_angle = 0;

	const int MOVE_DIV = 16;

public:
	bool Move(GameContext& context);

	void SetGridPosition(GameContext& context, DirectX::SimpleMath::Vector2 gridPos);

	DirectX::SimpleMath::Vector2 GetGridPosition()
	{
		return m_gridPosition;
	}

	void Initialize(GameContext& context)override;
	void Update(GameContext& context)override;
	void Render(GameContext& context)override;
};