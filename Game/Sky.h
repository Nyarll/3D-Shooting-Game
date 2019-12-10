#pragma once

#include "../Frameworks/Component.h"

class SkyComponent : public Component
{
public:
	void Render(GameContext& context)override;
};