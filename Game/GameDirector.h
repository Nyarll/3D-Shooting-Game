#pragma once

#include "FixedCameraComponent.h"

enum ActionState
{
	Idle = 0,
	PlayerTurn,
	EnemyTurn,
	TurnEnd
};

// <ゲーム進行統括クラス>
class GameDirector : public Component
{
private:
	ActionState m_nowAct;

public:
	virtual void Initialize(GameContext& context)override;
	virtual void Update(GameContext& context)override;
	virtual void Render(GameContext& context)override;

	virtual std::wstring GetName()const override
	{
		return L"GameDirector";
	}

	ActionState GetNowActionState()
	{
		return m_nowAct;
	}
};
