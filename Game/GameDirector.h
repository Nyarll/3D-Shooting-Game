#pragma once

#include "FixedCameraComponent.h"
#include "../Frameworks/GameSprite.h"

// <ゲーム進行統括クラス>
class GameDirector : public Component
{
private:
	std::unique_ptr<GameSpriteEffect>	m_hpBar;

public:
	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;

	virtual std::wstring GetName()const override
	{
		return L"GameDirector";
	}
};
