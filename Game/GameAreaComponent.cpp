
#include "GameAreaComponent.h"

#include "../Frameworks/ResourceManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


void GameAreaComponent::Initialize(GameContext & context)
{
	m_polygon = std::make_unique<GameSpritePolygon>();
	m_polygon->Load(context, L"Resources/Sprite/areaLimit2.png");
	//m_polygon->Load(context, L"Resources/Sprite/Flag_of_Austria-Hungary.png");
}

void GameAreaComponent::Render(GameContext & context)
{
	{
		DirectX::SimpleMath::Vector3 pos = { AreaLimit.center };
		pos.z = -0.5f;

		DirectX::SimpleMath::Matrix world = Matrix::CreateScale(AreaLimit.range.x);
		world *= Matrix::CreateTranslation(pos);

		RECT size = context.Get<DX::DeviceResources>().GetOutputSize();
		float aspectRatio = float(size.right) / float(size.bottom);
		float fovAngleY = XMConvertToRadians(45.0f);
		auto proj = Matrix::CreatePerspectiveFieldOfView(
			fovAngleY, aspectRatio,
			0.01f, 100.0f
		);

		auto& camera = context.GetFollowCamera();
		m_polygon->Draw(context, pos, world, camera.m_view, proj);

		pos = { AreaLimit.center };
		pos.z = -0.5f;
		world = Matrix::CreateScale(AreaLimit.range.x);
		world *= Matrix::CreateRotationY(XMConvertToRadians(180.f));
		world *= Matrix::CreateTranslation(pos);
		m_polygon->Draw(context, pos, world, camera.m_view, proj);

		pos = { AreaLimit.center };
		pos.z = 0.5f;
		world = Matrix::CreateScale(AreaLimit.range.x);
		world *= Matrix::CreateRotationY(XMConvertToRadians(90.f));
		world *= Matrix::CreateTranslation(pos);
		m_polygon->Draw(context, pos, world, camera.m_view, proj);

		pos = { AreaLimit.center };
		pos.z = 0.5f;
		world = Matrix::CreateScale(AreaLimit.range.x);
		world *= Matrix::CreateRotationY(XMConvertToRadians(-90.f));
		world *= Matrix::CreateTranslation(pos);
		m_polygon->Draw(context, pos, world, camera.m_view, proj);

		pos = { AreaLimit.center };
		pos.z = 0.5f;
		world = Matrix::CreateScale(AreaLimit.range.x);
		world *= Matrix::CreateRotationX(XMConvertToRadians(90.f));
		world *= Matrix::CreateTranslation(pos);
		m_polygon->Draw(context, pos, world, camera.m_view, proj);

		pos = { AreaLimit.center };
		pos.z = 0.5f;
		world = Matrix::CreateScale(AreaLimit.range.x);
		world *= Matrix::CreateRotationX(XMConvertToRadians(-90.f));
		world *= Matrix::CreateTranslation(pos);
		m_polygon->Draw(context, pos, world, camera.m_view, proj);
	}
}
