#pragma once

#include "../Frameworks/GameContext.h"
#include "../Frameworks/Component.h"
#include "../Frameworks/GridFloor.h"

class GridComponent : public Component
{
private:
	std::unique_ptr<GridFloor>	m_grid;

public:
	void Initialize(GameContext& context);
	void Render(GameContext& context);
};