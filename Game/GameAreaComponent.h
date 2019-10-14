#pragma once
#include "../Frameworks/Component.h"

#include "../Frameworks/GameSprite.h"

struct Area
{
	DirectX::SimpleMath::Vector3 center;
	DirectX::SimpleMath::Vector3 range;
};

class GameAreaComponent : public Component
{
private:
	std::unique_ptr<GameSpritePolygon> m_polygon;

public:
	static Area AreaLimit;

public:
	void Initialize(GameContext& context);

	//void Update(GameContext& context);

	void Render(GameContext& context);
};