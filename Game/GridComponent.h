#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/Component.h"
#include "../Frameworks/InfinityGridFloor.h"

class GridComponent : public Component
{
public:
	void Initialize(GameContext& context);
	void Render(GameContext& context);
};