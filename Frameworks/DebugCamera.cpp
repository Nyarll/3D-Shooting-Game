
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

	// <ƒrƒ…[s—ñ‚ðŽZo>

	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(-m_yAngle);
	DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(m_xAngle);
	// <‰ñ“]s—ñ>
	DirectX::SimpleMath::Matrix rt = rotY * rotX;

	Vector3 eye = offset;
	Vector3 target = { 0,0,0 };
	Vector3 up = { 0,1,0 };

	eye = DirectX::SimpleMath::Vector3::Transform(eye, rt.Invert());
	up = DirectX::SimpleMath::Vector3::Transform(up, rt.Invert());

	eye += targetTransform.position;

	targetTransform.position += move;
	target = targetTransform.position;

	// <ViewMatrix‚ð¶¬>
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

DebugFollowCamera::DebugFollowCamera(DirectX::SimpleMath::Vector3 eyePos,
	DirectX::SimpleMath::Vector3 targetPos,
	HWND handle)
{
	m_pos = eyePos;
	m_offset = eyePos;
	m_target = targetPos;

	window_handle = handle;

	DirectX::SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);
	m_view = Matrix::CreateLookAt(eyePos, targetPos, up);

	m_xAngle = m_yAngle = 0;
	m_x = m_y = 0;
}

DebugFollowCamera::~DebugFollowCamera()
{
}

void DebugFollowCamera::SetTarget(DirectX::SimpleMath::Vector3 target)
{
	m_target = target;
}

void DebugFollowCamera::Initialize(GameContext & context)
{
	RECT size = context.Get<DX::DeviceResources>().GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = XMConvertToRadians(45.0f);
	m_proj = Matrix::CreatePerspectiveFieldOfView(
		fovAngleY, aspectRatio,
		0.01f, 10000.0f
	);
}

void DebugFollowCamera::Update(GameContext & context)
{
	m_windowPos = GetWindowPosition(window_handle);
	RECT cr;
	GetClientRect(window_handle, &cr);
	float cx = m_windowPos.x + cr.right / 2;
	float cy = m_windowPos.y + cr.bottom / 2;

	auto state = context.Get<DirectX::Mouse>().GetState();

	{
		DirectX::SimpleMath::Vector2 offset = { 9.f,38.f };
		auto move = DirectX::SimpleMath::Vector2(
			state.x + m_windowPos.x + offset.x,
			state.y + m_windowPos.y + offset.y);
		Move(move);
	}
	SetCursorPos(cx, cy);
	context.Get<DirectX::Mouse>().SetVisible(false);
	m_x = cx;	m_y = cy;

	// <ƒrƒ…[s—ñ‚ðŽZo>
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(-m_yAngle);
	DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(m_xAngle);
	// <‰ñ“]s—ñ>
	DirectX::SimpleMath::Matrix rt = rotY * rotX;

	DirectX::SimpleMath::Vector3 eye = m_offset;
	DirectX::SimpleMath::Vector3 target = m_target;
	DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	// <eye ‚Í Vector3::Zero ‚ð’†S‚É‚µ‚Ä‰ñ“]‚·‚é>
	eye = DirectX::SimpleMath::Vector3::Transform(eye, rt.Invert());
	up = DirectX::SimpleMath::Vector3::Transform(up, rt.Invert());

	// <‰ñ“]‚µ‚½ŒãAˆÚ“®‚·‚é>
	eye += target;

	m_pos = eye;

	// <ViewMatrix‚ð¶¬>
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up);
}

void DebugFollowCamera::Finalize(GameContext & context)
{
}

void DebugFollowCamera::Move(const DirectX::SimpleMath::Vector2 & move)
{
	// <(‘Š‘Î’l)>
	float dx = move.x - static_cast<float>(m_x);
	float dy = move.y - static_cast<float>(m_y);

	if (dx != 0.0f || dy != 0.0f)
	{
		// <‚xŽ²‰ñ“]>
		float yAngle = dx * DirectX::XM_PI / 180.0f;
		// <‚wŽ²‰ñ“]>
		float xAngle = dy * DirectX::XM_PI / 180.0f;

		m_xAngle += xAngle / 10;
		m_yAngle -= yAngle / 10;

		//if (m_xAngle > (90.0f * DirectX::XM_PI / 180.0f))m_xAngle = (90.0f * DirectX::XM_PI / 180.0f);
		//if (m_xAngle < (-90.0f * DirectX::XM_PI / 180.0f))m_xAngle = (-90.0f * DirectX::XM_PI / 180.0f);
	}
}

DirectX::SimpleMath::Vector2 DebugFollowCamera::GetWindowPosition(HWND hwnd)
{
	{
		RECT r;
		GetWindowRect(hwnd, &r);
		DirectX::SimpleMath::Vector2 pos = { static_cast<float>(r.left), static_cast<float>(r.top) };
		return pos;
	}
}
