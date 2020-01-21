
#include "PlayerComponent.h"

#include "SceneManager.h"

#include "Stage.h"
#include "FixedCameraComponent.h"

#include "StatusComponent.h"

bool PlayerComponent::Move(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& stage = scene.Find(L"Stage")->GetComponent<Stage>();

	auto key = context.Get<DirectX::Keyboard>().GetState();
	bool is_move = false;

	if (!isMove)
	{
		if (key.IsKeyDown(DirectX::Keyboard::Right))
		{
			if (stage->IsPassable(static_cast<int>(m_gridPosition.x + 1.f), static_cast<int>(m_gridPosition.y)))
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition.x += 1;
				isMove = is_move = true;

				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };
				m_dir.x = 1;
				m_dir.y = 0;

				m_angle = DirectX::XMConvertToRadians(-90.f);
			}
		}
		else if (key.IsKeyDown(DirectX::Keyboard::Left))
		{
			if (stage->IsPassable(static_cast<int>(m_gridPosition.x - 1.f), static_cast<int>(m_gridPosition.y)))
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition.x -= 1;
				isMove = is_move = true;

				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };
				m_dir.x = -1;
				m_dir.y = 0;

				m_angle = DirectX::XMConvertToRadians(90.f);
			}
		}
		else if (key.IsKeyDown(DirectX::Keyboard::Down))
		{
			if (stage->IsPassable(static_cast<int>(m_gridPosition.x), static_cast<int>(m_gridPosition.y + 1.f)))
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition.y += 1;
				isMove = is_move = true;

				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };
				m_dir.x = 0;
				m_dir.y = 1;

				m_angle = DirectX::XMConvertToRadians(180.f);
			}
		}
		else if (key.IsKeyDown(DirectX::Keyboard::Up))
		{
			if (stage->IsPassable(static_cast<int>(m_gridPosition.x), static_cast<int>(m_gridPosition.y - 1.f)))
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition.y -= 1;
				isMove = is_move = true;

				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };
				m_dir.x = 0;
				m_dir.y = -1;

				m_angle = 0;
			}
		}
	}

	return is_move;
}

void PlayerComponent::SetGridPosition(GameContext & context, DirectX::SimpleMath::Vector2 gridPos)
{
	m_gridPosition = gridPos;
	m_gridOldPos = m_gridPosition;

	gameObject->transform->localPosition = { m_gridPosition.x, 0, m_gridPosition.y };
}

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
	if (isMove)
	{
		m_gridOldPos += m_v;
		gameObject->transform->localPosition = { m_gridOldPos.x, 0, m_gridOldPos.y };
		m_cnt++;
	}

	if (m_cnt == MOVE_DIV)
	{
		m_v = DirectX::SimpleMath::Vector2::Zero;
		isMove = false;
		m_cnt = 0;
	}
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
