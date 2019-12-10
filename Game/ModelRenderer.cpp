
#include "ModelRenderer.h"

#include "SceneManager.h"
#include "GameCameraComponent.h"

void ModelRenderer::Initialize(GameContext & context)
{

}

void ModelRenderer::Update(GameContext & context)
{
}

void ModelRenderer::Render(GameContext & context)
{
	auto& dr = context.GetDR();
	auto& camera = context.Get<SceneManager>().GetActiveScene().Find(L"Camera")->GetComponent<FixedCamera>();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(gameObject->transform->localScale);
	world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(gameObject->transform->localRotation);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(gameObject->transform->localPosition);
	m_model->Draw(dr.GetD3DDeviceContext(), context.Get<DirectX::CommonStates>(),
		world, camera->GetViewMatrix(), camera->GetProjectionMatrix());
}
