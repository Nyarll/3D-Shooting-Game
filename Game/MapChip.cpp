
#include "MapChip.h"

MapChip::MapChip(float x, float y, bool isPassable)
{
	m_position = DirectX::SimpleMath::Vector3(x, 0.f, y);
	m_isPassable = isPassable;
}
