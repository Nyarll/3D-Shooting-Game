#pragma once

#include "ICameraComponent.h"

class FixedCamera : public ICameraComponent
{
private:
	DirectX::SimpleMath::Vector3 m_eye;
	DirectX::SimpleMath::Vector3 m_target;

public:
	FixedCamera();

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;


private:
	void Move(GameContext& context);

public:
	const DirectX::SimpleMath::Vector3& GetEyePosition() { return m_eye; }
	const DirectX::SimpleMath::Vector3& GetTargetPosition() { return m_target; }
};