#pragma once

#include "../Frameworks/GameContext.h"

// <Scene Interface>
class IScene
{
public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual void Initialize(GameContext& context) = 0;
	virtual void Update(GameContext& context) = 0;
	virtual void Render(GameContext& context) = 0;
	virtual void Finalize(GameContext& context) = 0;
};