
#include "GridComponent.h"

#include "GameAreaComponent.h"

void GridComponent::Initialize(GameContext & context)
{
	context.Register(std::make_unique<InfinityGridFloor>());
	context.Get<InfinityGridFloor>().init(context, 1.f, { GameAreaComponent::AreaLimit.range.x, GameAreaComponent::AreaLimit.range.z });
}

void GridComponent::Render(GameContext & context)
{
	context.Get<InfinityGridFloor>().draw(context);
}

