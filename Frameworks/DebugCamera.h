#pragma once
#include "Camera.h"
#include "Transform.h"

// <デバッグ用カメラ>
class DebugCamera : public ICamera
{
private:
	float m_xAngle, m_yAngle;
	float m_xAngleLast, m_yAngleLast;

	Transform myTransform, targetTransform;

	DirectX::SimpleMath::Vector3 offset;

	DirectX::SimpleMath::Vector3 m_acc, m_vel;

public:
	DebugCamera() = default;
	virtual ~DebugCamera() = default;

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Finalize(GameContext& context)override;

private:
	void Move(int x, int y);
};