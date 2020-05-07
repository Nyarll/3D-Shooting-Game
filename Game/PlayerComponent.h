#pragma once

#include "../Frameworks/Component.h"
#include "../Frameworks/GameContext.h"

class PlayerComponent : public Component
{
private:

	std::unique_ptr<DirectX::GeometricPrimitive>	m_geo;

	DirectX::SimpleMath::Vector2 m_dir;
	float m_angle = 0;

public:

	void Initialize(GameContext& context)override;
	void Update(GameContext& context)override;
	void Render(GameContext& context)override;
};