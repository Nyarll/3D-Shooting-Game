#pragma once

#include "../Frameworks/Component.h"
#include "MapChip.h"

class Stage : public Component
{
private:
	int m_x, m_y;	// <ステージサイズ(デフォルトサイズは10)>

	std::vector<std::vector<MapChip>>	m_stage;	// <ステージデータ>

	std::unique_ptr<DirectX::Model>	m_model;

public:
	Stage();

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;

	void SetSize(int x, int y)
	{
		m_x = x;
		m_y = y;
	}

	bool IsPassable(int x, int y)
	{
		if (x == m_x)
			return false;
		if (y == m_y)
			return false;
		if (x < 0)
			return false;
		if (y < 0)
			return false;

		return m_stage[y][x].IsPassable();
	}

	DirectX::SimpleMath::Vector2 GetStageSize()
	{
		return{ (float)m_x, (float)m_y };
	}
};