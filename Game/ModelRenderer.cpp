
#include "ModelRenderer.h"

#include "SceneManager.h"
#include "FixedCameraComponent.h"

void CMOModelRenderer::Initialize(GameContext & context)
{

}

void CMOModelRenderer::Update(GameContext & context)
{
}

void CMOModelRenderer::Render(GameContext & context)
{
	auto& dr = context.GetDR();
	auto& camera = context.Get<SceneManager>().GetActiveScene().Find(L"Camera")->GetComponent<FixedCamera>();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(gameObject->transform->localScale);
	world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(gameObject->transform->localRotation);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(gameObject->transform->localPosition);
	m_model->Draw(dr.GetD3DDeviceContext(), context.Get<DirectX::CommonStates>(),
		world, camera->GetViewMatrix(), camera->GetProjectionMatrix());
}
