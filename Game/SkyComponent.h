#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/Component.h"

class SkyComponent : public Component
{
public:
	void Update(GameContext& context);
	void Render(GameContext& context);
};