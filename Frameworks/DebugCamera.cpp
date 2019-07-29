#include "pch.h"

#include "../DeviceResources.h"
#include "GameContext.h"
#include "DebugCamera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void DebugCamera::Initialize(GameContext & context)
{
	DirectX::SimpleMath::Vector3 eye(0.0f, 2.0f, -10.0f);
	m_eye = eye;
	offset = eye;
	myTransform.position = eye;
	DirectX::SimpleMath::Vector3 target = Vector3::Zero;
	m_target = target;
	targetTransform.position = target;
	DirectX::SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);
	m_view = Matrix::CreateLookAt(eye, target, up);

	RECT size = context.Get<DX::DeviceResources>().GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = XMConvertToRadians(45.0f);
	m_proj = Matrix::CreatePerspectiveFieldOfView(
		fovAngleY, aspectRatio,
		0.01f, 10000.0f
	);

	//context.Get<Mouse>().SetMode(Mouse::Mode::MODE_RELATIVE);
}

void DebugCamera::Update(GameContext & context)
{
	auto& mouse = context.Get<Mouse>();
	auto last_state = context.Get<Mouse::ButtonStateTracker>().GetLastState();
	auto state = context.Get<Mouse>().GetState();

	auto key = context.Get<Keyboard>().GetState();

	
	//if (state.positionMode == DirectX::Mouse::MODE_RELATIVE)return;

	//Move(state.x, state.y);

	Vector3 move = { 0,0,0 };
	if (key.W)move.z += 0.05f;
	if (key.S)move.z -= 0.05f;

	if (key.A)move.x += 0.05f;
	if (key.D)move.x -= 0.05f;

	// <ビュー行列を算出>

	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(-m_yAngle);
	DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(m_xAngle);
	// <回転行列>
	DirectX::SimpleMath::Matrix rt = rotY * rotX;

	Vector3 eye = offset;
	Vector3 target = { 0,0,0 };
	Vector3 up = { 0,1,0 };

	eye = DirectX::SimpleMath::Vector3::Transform(eye, rt.Invert());
	up = DirectX::SimpleMath::Vector3::Transform(up, rt.Invert());

	eye += targetTransform.position;

	targetTransform.position += move;
	target = targetTransform.position;

	// <ViewMatrixを生成>
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up);
}

void DebugCamera::Finalize(GameContext & context)
{
}

void DebugCamera::Move(int x, int y)
{
	float dx = static_cast<float>(x);
	float dy = static_cast<float>(y);
	if (x != 0.0f || y != 0.0f)
	{
		float yAngle = DirectX::XMConvertToRadians(dx);
		float xAngle = DirectX::XMConvertToRadians(dy);

		m_xAngle += xAngle;
		m_yAngle += yAngle;

		if (m_xAngle > (DirectX::XMConvertToRadians(90.0f)))m_xAngle = DirectX::XMConvertToRadians(90.0f);
		if (m_xAngle < (DirectX::XMConvertToRadians(-90.0f)))m_xAngle = DirectX::XMConvertToRadians(-90.0f);
	}
}
