
#include "PlayerComponent.h"

#include "SceneManager.h"

#include "Stage.h"
#include "FixedCameraComponent.h"

#include "StatusComponent.h"

void PlayerComponent::Initialize(GameContext & context)
{
	m_geo = DirectX::GeometricPrimitive::CreateTeapot(
		context.GetDR().GetD3DDeviceContext(),
		0.75f
	);

	m_dir = { 0,1 };
}

void PlayerComponent::Update(GameContext & context)
{
	
}

void PlayerComponent::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), m_angle);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(gameObject->transform->position);
	m_geo->Draw(world, camera->GetViewMatrix(), camera->GetProjectionMatrix(), DirectX::Colors::Blue);
}
