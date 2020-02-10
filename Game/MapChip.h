#pragma once

// <マップチップ>
class MapChip
{
private:
	DirectX::SimpleMath::Vector3 m_position;
	bool m_isPassable;

	int m_drawPrio = 0;

public:
	MapChip(float x, float y, bool isPassable);

	const DirectX::SimpleMath::Vector3& GetPosition() { return m_position; }
	bool IsPassable() { return m_isPassable; }

	void SetPassable(bool passable) { m_isPassable = passable; }

	void SetDrawPrio(int prio) { m_drawPrio = prio; }
	int GetDrawPrio()const { return m_drawPrio; }
};