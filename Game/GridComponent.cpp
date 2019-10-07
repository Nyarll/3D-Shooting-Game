
#include "GridComponent.h"

void GridComponent::Initialize(GameContext & context)
{
	context.Register(std::make_unique<InfinityGridFloor>());
	context.Get<InfinityGridFloor>().init(context, 1.f, { 300.f,300.f });
}

void GridComponent::Render(GameContext & context)
{
	context.Get<InfinityGridFloor>().draw(context);
}

