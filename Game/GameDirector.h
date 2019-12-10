#pragma once

#include "FixedCameraComponent.h"

class GameDirector : public Component
{
private:
	std::unique_ptr<DirectX::GeometricPrimitive>	m_geo;

public:
	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;

	virtual std::wstring GetName()const override
	{
		return L"GameDirector";
	}
};
