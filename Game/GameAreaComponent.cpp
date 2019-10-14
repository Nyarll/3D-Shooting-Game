
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
		DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
		DirectX::SimpleMath::Vector3 pos = { this->AreaLimit.center };
		m_polygon->Draw(context, pos, world);
	}
}
