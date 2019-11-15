#pragma once

#include <SimpleMath.h>

#include "Object.h"

class GameContext;

// <カメラのインターフェイス>
class ICamera : public Object
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

	DirectX::SimpleMath::Vector3 GetCameraDirection()
	{
		return{ m_target - m_eye };
	}

	DirectX::SimpleMath::Ray ViewportPointToRay(const DirectX::SimpleMath::Vector3& point) const
	{
		auto pointNear = DirectX::SimpleMath::Vector3(point.x, point.y, 0.0f);
		auto pointFar = DirectX::SimpleMath::Vector3(point.x, point.y, 1.0f);

		auto inverseviewproj = (m_view * m_proj).Invert();
		auto rayNear = DirectX::SimpleMath::Vector3::Transform(pointNear, inverseviewproj);
		auto rayFar = DirectX::SimpleMath::Vector3::Transform(pointFar, inverseviewproj);
		auto raySubtraction = rayFar - rayNear;
		auto rayDirection = raySubtraction;
		//rayDirection.Normalize();

		return DirectX::SimpleMath::Ray(DirectX::SimpleMath::Vector3(rayNear), DirectX::SimpleMath::Vector3(rayDirection));
	}

	virtual std::wstring GetType()const
	{
		return L"Camera";
	}

	virtual std::wstring GetName()const
	{
		return L"Camera";
	}
};
