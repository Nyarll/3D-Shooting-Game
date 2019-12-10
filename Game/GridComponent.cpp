
#include "GridComponent.h"
#include "../Frameworks/GameObject.h"

#include "SceneManager.h"
#include "GameCameraComponent.h"

void GridComponent::Initialize(GameContext & context)
{
	auto& dr = context.GetDR();
	int floorSize = 10;
	m_grid = std::make_unique<GridFloor>(dr.GetD3DDevice(),
		dr.GetD3DDeviceContext(),
		context.GetPtr<DirectX::CommonStates>(),
		static_cast<float>(floorSize), floorSize); 
}

void GridComponent::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	m_grid->draw(context.GetDR().GetD3DDeviceContext(),
		camera->GetViewMatrix(), camera->GetProjectionMatrix());
}

