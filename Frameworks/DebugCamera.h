#pragma once
#include "Camera.h"
#include "Transform.h"

// <�f�o�b�O�p�J����>
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

// <�Ǐ]�J����>
class DebugFollowCamera : public ICamera
{
private:
	// <��]>
	float m_yAngle, m_xAngle;

	// <�O��̃}�E�X���W>
	int m_x, m_y;

	// <����>
	float m_length;

	// <�X�N���[���z�C�[���l>
	int m_scrollWheelValue;

	DirectX::SimpleMath::Vector3 m_pos;

	DirectX::SimpleMath::Vector3 m_eyeRef;
	DirectX::SimpleMath::Vector3 m_targetRef;

	DirectX::SimpleMath::Vector3 m_offset;

	HWND window_handle;
	DirectX::SimpleMath::Vector2 m_windowPos;

public:
	DebugFollowCamera(DirectX::SimpleMath::Vector3 eyePos,
		DirectX::SimpleMath::Vector3 targetPos,
		HWND handle);
	virtual ~DebugFollowCamera();

	void SetTarget(DirectX::SimpleMath::Vector3 target);

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Finalize(GameContext& context)override;

private:
	void Move(const DirectX::SimpleMath::Vector2& move);

	DirectX::SimpleMath::Vector2 GetWindowPosition(HWND hwnd);
};