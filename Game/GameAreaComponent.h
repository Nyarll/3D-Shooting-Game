#pragma once
#include "../Frameworks/Component.h"

#include "../Frameworks/GameSprite.h"

struct
{
	DirectX::SimpleMath::Vector3 center = { 0,0,0 };
	DirectX::SimpleMath::Vector3 range = { 1000,1000,1000 };
}const AreaLimit;

class GameAreaComponent : public Component
{
private:
	std::unique_ptr<GameSpritePolygon> m_polygon;

public:
	void Initialize(GameContext& context);

	void Render(GameContext& context);
};