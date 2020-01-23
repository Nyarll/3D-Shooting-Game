
#include "EnemyComponent.h"

#include "PlayerComponent.h"

#include "SceneManager.h"

#include "Stage.h"
#include "FixedCameraComponent.h"

#include "../Frameworks/Astar.hpp"

#include "StatusComponent.h"

#include "../Frameworks/JsonImport.h"

#include "../Frameworks/Random.h"

bool EnemyComponent::Move(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& stage = scene.Find(L"Stage")->GetComponent<Stage>();
	auto& player = scene.Find(L"Player")->GetComponent<PlayerComponent>();

	m_direction = static_cast<Direction>(this->GetMovingDirection(context));

	DirectX::SimpleMath::Vector2 vel;

	switch (m_direction)
	{
	case Direction::Left:
		vel = { -1,0 };
		break;

	case Direction::Right:
		vel = { 1,0 };
		break;

	case Direction::Up:
		vel = { 0,-1 };
		break;

	case Direction::Down:
		vel = { 0, 1 };
		break;
	}

	m_gridPosition += vel;

	return true;



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
	m_direction = Direction::Down;
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

	DirectX::SimpleMath::Vector3 pos(static_cast<float>((int)(m_gridPosition.x)), 0, static_cast<float>((int)(m_gridPosition.y)));

	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), m_angle);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(pos));
	m_geo->Draw(world, camera->GetViewMatrix(), camera->GetProjectionMatrix(), DirectX::Colors::Red);
}

void EnemyComponent::GenerateRoute(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& stage = scene.Find(L"Stage")->GetComponent<Stage>();
	Random random;
	DirectX::SimpleMath::Vector2 nextPoint = DirectX::SimpleMath::Vector2::Zero;
	while (true)
	{
		int x = random.Range(0, stage->GetStageSize().x);
		int y = random.Range(0, stage->GetStageSize().y);
		nextPoint = { static_cast<float>(x),static_cast<float>(y) };
		if (stage->IsPassable(x, y) && m_gridPosition != nextPoint)
		{
			break;
		}
	}
	AStar::Generator generator;
	int mapX = static_cast<int>(stage->GetStageSize().x);
	int mapY = static_cast<int>(stage->GetStageSize().y);

	generator.SetWorldSize({ mapX,mapY });
	generator.SetHeuristic(AStar::Heuristic::euclidean);
	generator.SetDiagonalMovement(false);

	for (int y = 0; y < mapY; y++)
	{
		for (int x = 0; x < mapX; x++)
		{
			if (!stage->IsPassable(x, y))
			{
				generator.AddCollision({ x,y });
			}
		}
	}

	auto path = generator.FindPath({ static_cast<int>(m_gridPosition.x),static_cast<int>(m_gridPosition.y) },
	{ static_cast<int>(nextPoint.x),static_cast<int>(nextPoint.y) });

	for (auto& coordinate : path)
	{
		DirectX::SimpleMath::Vector2 p(static_cast<float>(coordinate.x), static_cast<float>(coordinate.y));
		m_route.push_back(p);
	}
}

int EnemyComponent::GetMovingDirection(GameContext& context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& stage = scene.Find(L"Stage")->GetComponent<Stage>();

	int ex = static_cast<int>(m_gridPosition.x);
	int ey = static_cast<int>(m_gridPosition.y);

	Direction gotoDirection = Direction::Left;

	if (m_route.size() > 0)
	{
		for (int i = 0; i < m_route.size(); i++)
		{
			if (static_cast<int>(m_route[i].x) == (static_cast<int>(this->m_gridPosition.x) - 1) &&
				static_cast<int>(m_route[i].y) == static_cast<int>(this->m_gridPosition.y))
			{
				return Left;
			}
			if (static_cast<int>(m_route[i].x) == (static_cast<int>(this->m_gridPosition.x) + 1) &&
				static_cast<int>(m_route[i].y) == static_cast<int>(this->m_gridPosition.y))
			{
				return Right;
			}
			if ((static_cast<int>(m_route[i].x)) == static_cast<int>(this->m_gridPosition.x) &&
				static_cast<int>(m_route[i].y) == (static_cast<int>(this->m_gridPosition.y) - 1))
			{
				return Up;
			}
			if ((static_cast<int>(m_route[i].x)) == static_cast<int>(this->m_gridPosition.x) &&
				static_cast<int>(m_route[i].y) == (static_cast<int>(this->m_gridPosition.y) + 1))
			{
				return Down;
			}
		}

		m_route.erase(m_route.begin());
	}
	else
	{
		this->GenerateRoute(context);
	}

	return this->GetMovingDirection(context);
}
