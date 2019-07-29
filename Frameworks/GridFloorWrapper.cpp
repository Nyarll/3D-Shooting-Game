#include "pch.h"

#include "GridFloorWrapper.h"
#include "GameContext.h"
#include "DebugCamera.h"

void GridFloorWrapper::Initialize(GameContext& context)
{
	auto& deviceResources = context.Get<DX::DeviceResources>();
	// <ƒOƒŠƒbƒh°‚Ìì¬>
	m_gridFloor = std::make_unique<GridFloor>(deviceResources.GetD3DDevice(), deviceResources.GetD3DDeviceContext(),
		&context.Get<DirectX::CommonStates>(), 10.0f, 10);
}

void GridFloorWrapper::Render(GameContext& context)
{
	auto& deviceResources = context.Get<DX::DeviceResources>();
	m_gridFloor->draw(deviceResources.GetD3DDeviceContext(), context.Get<DebugCamera>().m_view, context.Get<DebugCamera>().m_proj);
}