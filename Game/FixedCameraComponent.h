#pragma once

#include "ICameraComponent.h"

class FixedCamera : public ICameraComponent
{
private:
	enum MODE
	{
		Normal,
		FPS,
	};

private:
	DirectX::SimpleMath::Vector3 m_eye;
	DirectX::SimpleMath::Vector3 m_target;

	GameObject* m_targetObject = nullptr;

	MODE mode = MODE::Normal;

public:
	FixedCamera();

	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;


private:
	void Move(GameContext& context);

public:
	const DirectX::SimpleMath::Vector3& GetEyePosition() { return m_eye; }
	const DirectX::SimpleMath::Vector3& GetTargetPosition() { return m_target; }

	void SetTarget(GameObject* targetObject)
	{
		m_targetObject = targetObject;
	}

	void ModeNormal() { mode = MODE::Normal; }
	void ModeFPS() { mode = MODE::FPS; }
};