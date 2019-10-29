#pragma once

#include "../Frameworks/Component.h"
#include "../Frameworks/GameObject.h"

class PlayerComponent : public Component
{
private:
	const int ShotDelayCount = 10;
	const float MaxSpeed = 1.5f;
	const float MinSpeed = 0.01f;

private:
	enum playerState
	{
		Idle,
		Walk,
		RightWalk,
		LeftWalk,
		Back,
		BreakDance
	}m_playerState;

	DirectX::SimpleMath::Vector3 m_accele, m_velocity;

	int m_shotCount = 0;
	float m_forwardSpeed = 0.0f;

public:
	void Initialize(GameContext& context);

	void Update(GameContext& context);

	void Render(GameContext& context);

	float GetSpeed() { return m_forwardSpeed; }
	DirectX::SimpleMath::Vector3 GetLocalPosition() { return gameObject->transform->localPosition; }

private:
	void Fire(GameContext& context, DirectX::SimpleMath::Vector3 shot_pos);
};