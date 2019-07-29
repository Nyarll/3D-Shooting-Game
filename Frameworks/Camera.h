#pragma once

#include <SimpleMath.h>

class GameContext;

// <カメラのインターフェイス>
class ICamera
{
public:
	DirectX::SimpleMath::Vector3 m_eye;
	DirectX::SimpleMath::Vector3 m_target;

	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

public:
	ICamera() = default;
	virtual ~ICamera() = default;

	virtual void Initialize(GameContext& context) = 0;
	virtual void Update(GameContext& context) = 0;
	virtual void Finalize(GameContext& context) = 0;
};
