#pragma once

#include "../Frameworks/Component.h"
#include "../Frameworks/ResourceManager.h"

class NormalBulletComponent : public Component
{
private:
	int m_lifeTime;
	DirectX::SimpleMath::Vector3 m_vel;
	DirectX::SimpleMath::Vector3 m_direction;

	float m_angle = 0.f;
	DirectX::SimpleMath::Matrix rotate;

public:
	void Initialize(GameContext& context)override;
	void Update(GameContext& context)override;
	void Render(GameContext& context)override;

	void SetDirection(const DirectX::SimpleMath::Vector3& direction);
	void SetStartPosition(const DirectX::SimpleMath::Vector3& position);
};