
#include "PlayerComponent.h"

#include "SceneManager.h"

#include "Stage.h"
#include "FixedCameraComponent.h"

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
			}
		}
		if (key.IsKeyDown(DirectX::Keyboard::Left))
		{
			if (stage->IsPassable(static_cast<int>(m_gridPosition.x - 1.f), static_cast<int>(m_gridPosition.y)))
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition.x -= 1;
				isMove = is_move = true;

				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };
			}
		}
		if (key.IsKeyDown(DirectX::Keyboard::Down))
		{
			if (stage->IsPassable(static_cast<int>(m_gridPosition.x), static_cast<int>(m_gridPosition.y + 1.f)))
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition.y += 1;
				isMove = is_move = true;

				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };
			}
		}
		if (key.IsKeyDown(DirectX::Keyboard::Up))
		{
			if (stage->IsPassable(static_cast<int>(m_gridPosition.x), static_cast<int>(m_gridPosition.y - 1.f)))
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition.y -= 1;
				isMove = is_move = true;

				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };
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
	m_geo = DirectX::GeometricPrimitive::CreateSphere(
		context.GetDR().GetD3DDeviceContext(),
		0.5f
	);
}

void PlayerComponent::Update(GameContext & context)
{

	m_gridOldPos += m_v;
	gameObject->transform->localPosition = { m_gridOldPos.x, 0, m_gridOldPos.y };

	if (m_gridOldPos == m_gridPosition)
	{
		m_v = DirectX::SimpleMath::Vector2::Zero;
		isMove = false;
	}
}

void PlayerComponent::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateTranslation(gameObject->transform->position);
	m_geo->Draw(world, camera->GetViewMatrix(), camera->GetProjectionMatrix(), DirectX::Colors::Blue);
}
