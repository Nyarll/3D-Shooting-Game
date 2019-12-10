
#include "FixedCameraComponent.h"

FixedCamera::FixedCamera()
{
}

void FixedCamera::Initialize(GameContext & context)
{
	m_eye = DirectX::SimpleMath::Vector3(0.f, 5.f, 5.f);
	m_target = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
}

void FixedCamera::Update(GameContext & context)
{
	Move(context);
	// <View Matrix>
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_eye, m_target, DirectX::SimpleMath::Vector3::Up);

	// <Projection Matrix>
	RECT size = context.Get<DX::DeviceResources>().GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = DirectX::XMConvertToRadians(45.0f);
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		fovAngleY, aspectRatio,
		0.01f, 10000.0f
	);
}

void FixedCamera::Move(GameContext& context)
{
	auto key = context.Get<DirectX::Keyboard>().GetState();
	DirectX::SimpleMath::Vector3 move = DirectX::SimpleMath::Vector3::Zero;
	if (key.A)
	{
		move.x -= 1;
	}
	if (key.D)
	{
		move.x += 1;
	}
	if (key.W)
	{
		move.z -= 1;
	}
	if (key.S)
	{
		move.z += 1;
	}
	move.Normalize();
	move *= 0.05f;

	m_eye += move;
	m_target += move;
}
