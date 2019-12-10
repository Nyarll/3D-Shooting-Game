
#include "GameDirector.h"
#include "SceneManager.h"

void GameDirector::Initialize(GameContext & context)
{
	m_geo = DirectX::GeometricPrimitive::CreateSphere(
		context.GetDR().GetD3DDeviceContext(),
		0.1f
	);
}

void GameDirector::Update(GameContext & context)
{
}

void GameDirector::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto camera = scene.Find(L"Camera")->GetComponent<GameCameraComponent>();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateTranslation(camera->GetTargetPosition());
	m_geo->Draw(world, camera->GetViewMatrix(), camera->GetProjectionMatrix(), DirectX::Colors::Red);
}
