
#include "GameAreaComponent.h"

#include "../Frameworks/ResourceManager.h"

Area GameAreaComponent::AreaLimit = { {0,0,0},{2000,2000,2000} };

void GameAreaComponent::Initialize(GameContext & context)
{
	m_polygon = std::make_unique<GameSpritePolygon>();
	m_polygon->Load(context, L"../Resources/Sprite/areaLimit.png");
}

void GameAreaComponent::Render(GameContext & context)
{
	{
		DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateTranslation(AreaLimit.range.x / 2, AreaLimit.range.y / 2, AreaLimit.range.z / 2);
		m_polygon->Draw(context, world);
	}
}
