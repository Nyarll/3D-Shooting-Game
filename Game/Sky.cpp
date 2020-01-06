
#include "Sky.h"

#include "../Frameworks/GameContext.h"

#include "SceneManager.h"

#include "FixedCameraComponent.h"

#include "../Frameworks/ResourceManager.h"

void SkyComponent::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(10.f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(gameObject->transform->localPosition);

	/*context.Get<ResourceManager>().GetFbxModel(ResourceManager::ResourceID::SkyDome).lock()->Draw(context.GetDR().GetD3DDevice(),
		context.GetDR().GetD3DDeviceContext(), world, camera->GetViewMatrix(), camera->GetProjectionMatrix());*/
}
