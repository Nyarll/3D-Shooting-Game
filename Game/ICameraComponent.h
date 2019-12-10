#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/Component.h"

class ICameraComponent : public Component
{
protected:
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

public:
	virtual std::wstring GetName()const override
	{
		return L"CameraComponent";
	}

	const DirectX::SimpleMath::Matrix& GetViewMatrix() { return m_view; }
	const DirectX::SimpleMath::Matrix& GetProjectionMatrix() { return m_proj; }
};