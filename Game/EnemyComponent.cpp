
#include "EnemyComponent.h"

#include "PlayerComponent.h"

#include "SceneManager.h"

#include "Stage.h"
#include "FixedCameraComponent.h"

#include "../Frameworks/Astar.hpp"

#include "StatusComponent.h"

#include "../Frameworks/JsonImport.h"


bool EnemyComponent::Move(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& stage = scene.Find(L"Stage")->GetComponent<Stage>();
	auto& player = scene.Find(L"Player")->GetComponent<PlayerComponent>();

	// <現在向いている方向が通行可能かどうか>
	auto vec = DirectX::SimpleMath::Vector2(static_cast<int>(m_gridPosition.x + m_dir.x), static_cast<int>(m_gridPosition.y + m_dir.y));
	if (stage->IsPassable(vec.x, vec.y))
	{
		auto pos = player->GetGridPosition();
		// <プレイヤーがいるかどうか>
		if (vec != pos)
		{
			bool isOver = false;
			auto& enemys = scene.FindAll(L"Enemy");
			for (int i = 0; i < enemys.size(); i++)
			{
				DirectX::SimpleMath::Vector2 epos = enemys[i]->GetComponent<EnemyComponent>()->GetGridPosition();
				if (vec == epos)
				{
					isOver = true;
					break;
				}
			}
			if (!isOver)
			{
				m_gridOldPos = m_gridPosition;
				m_gridPosition += m_dir;
				float mx = (m_gridPosition.x - m_gridOldPos.x) / MOVE_DIV;
				float my = (m_gridPosition.y - m_gridOldPos.y) / MOVE_DIV;

				m_v = { mx, my };

				isMove = true;
				return true;
			}
		}
	}

	float deg = DirectX::XMConvertToDegrees(m_angle);
	m_angle = DirectX::XMConvertToRadians(deg + 90.f);

	// ------<ここらへんに位置ズレの原因があると思われる>------
	m_dir.x = cosf(DirectX::XMConvertToRadians(deg));
	m_dir.y = sinf(DirectX::XMConvertToRadians(deg));

	m_dir.Normalize();
	Move(context);

	return false;
}

void EnemyComponent::SetGridPosition(GameContext & context, DirectX::SimpleMath::Vector2 gridPos)
{
	m_gridPosition = gridPos;
	m_gridOldPos = m_gridPosition;

	gameObject->transform->localPosition = { m_gridPosition.x, 0, m_gridPosition.y };
}

void EnemyComponent::Initialize(GameContext & context)
{
	m_geo = DirectX::GeometricPrimitive::CreateTeapot(
		context.GetDR().GetD3DDeviceContext(),
		0.75f
	);

	m_dir = { 0,1 };
	m_angle = DirectX::XMConvertToRadians(180.f);

	
	std::ifstream ifs(jsonFileName);

	if (!ifs)
	{
		gameObject->AddComponent<StatusComponent>();
		return;
	}

	picojson::value value;
	ifs >> value;

	std::string name = JsonImport::get_as<std::string>(
		JsonImport::get_value(value, "Name")
		);

	int HP = JsonImport::get_as<int>(JsonImport::get_value(value, "HP"));
	int ATK = JsonImport::get_as<int>(JsonImport::get_value(value, "ATK"));
	int DEF = JsonImport::get_as<int>(JsonImport::get_value(value, "DEF"));

	gameObject->AddComponent<StatusComponent>(name, HP, ATK, DEF);
}

void EnemyComponent::Update(GameContext & context)
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

void EnemyComponent::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	DirectX::SimpleMath::Vector3 pos(m_gridPosition.x, 0, m_gridPosition.y);

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
	//world *= DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), m_angle);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(pos));
	m_geo->Draw(world, camera->GetViewMatrix(), camera->GetProjectionMatrix(), DirectX::Colors::Red);
}
