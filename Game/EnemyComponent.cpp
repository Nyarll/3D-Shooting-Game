
#include "EnemyComponent.h"

#include "PlayerComponent.h"

#include "SceneManager.h"

#include "Stage.h"
#include "FixedCameraComponent.h"

#include "../Frameworks/Astar.hpp"

#include "StatusComponent.h"

#include "../Frameworks/JsonImport.h"

#include "../Frameworks/Random.h"

#include "../GameSystem.h"

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

int EnemyComponent::GetMovingDirection(GameContext& context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& player = scene.Find(L"Player")->GetComponent<PlayerComponent>();
	auto& stage = scene.Find(L"Stage")->GetComponent<Stage>();

	int searchArea = 3;
	int ex = static_cast<int>(this->GetGridPosition().x);
	int ey = static_cast<int>(this->GetGridPosition().y);
	int tx = static_cast<int>(player->GetGridPosition().x);
	int ty = static_cast<int>(player->GetGridPosition().y);

	if ((ex - searchArea <= tx) && (ex + searchArea >= tx) &&
		(ey - searchArea <= ty) && (ey + searchArea >= ty))
	{
		AStar::Generator generator;
		int MapSizeX = static_cast<int>(stage->GetStageSize().x);
		int MapSizeY = static_cast<int>(stage->GetStageSize().y);
		generator.SetWorldSize({ MapSizeX,MapSizeY });
		generator.SetHeuristic(AStar::Heuristic::euclidean);
		generator.SetDiagonalMovement(false);	// 今回は斜め移動しないので false

												// 壁を Astar アルゴリズムへ登録する
		for (int y = 0; y < MapSizeY; y++)
		{
			for (int x = 0; x < MapSizeX; x++)
			{
				if (!stage->IsPassable(x, y))
				{
					generator.AddCollision({ x,y });
				}
			}
		}

		std::vector<DirectX::SimpleMath::Vector2> data;	// ルート情報
		{
			auto path = generator.FindPath({ static_cast<int>(this->GetGridPosition().x), static_cast<int>(this->GetGridPosition().y) },
			{ static_cast<int>(player->GetGridPosition().x), static_cast<int>(player->GetGridPosition().y) });

			for (auto& coordinate : path)
			{
				DirectX::SimpleMath::Vector2 back(coordinate.x, coordinate.y);
				data.push_back(back);
			}
		}
		// 移動方向の決定
		for (int i = 0; i < data.size(); i++)
		{
			// 終端
			if (static_cast<int>(data[i].x) == static_cast<int>(this->GetGridPosition().x) &&
				static_cast<int>(data[i].y) == static_cast<int>(this->GetGridPosition().y))
			{
				break;
			}

			if (static_cast<int>(data[i].x) == (static_cast<int>(this->GetGridPosition().x) - 1) &&
				static_cast<int>(data[i].y) == static_cast<int>(this->GetGridPosition().y))
			{
				return Left;
			}
			if (static_cast<int>(data[i].x) == (static_cast<int>(this->GetGridPosition().x) + 1) &&
				static_cast<int>(data[i].y) == static_cast<int>(this->GetGridPosition().y))
			{
				return Right;
			}
			if ((static_cast<int>(data[i].x)) == static_cast<int>(this->GetGridPosition().x) &&
				static_cast<int>(data[i].y) == (static_cast<int>(this->GetGridPosition().y) - 1))
			{
				return Up;
			}
			if ((static_cast<int>(data[i].x)) == static_cast<int>(this->GetGridPosition().x) &&
				static_cast<int>(data[i].y) == (static_cast<int>(this->GetGridPosition().y) + 1))
			{
				return Down;
			}
		}
	}
	else
	{
		int GotoDirection = Left;
		switch (m_direction)
		{
		case Left:
			if (stage->IsPassable(ex - 1, ey))
			{
				GotoDirection = Left;
				break;
			}
			if (stage->IsPassable(ex, ey + 1))
			{
				GotoDirection = Down;
				break;
			}
			if (stage->IsPassable(ex, ey - 1))
			{
				GotoDirection = Up;
				break;
			}


		case Right:
			if (stage->IsPassable(ex + 1, ey))
			{
				GotoDirection = Right;
				break;
			}
			if (stage->IsPassable(ex, ey - 1))
			{
				GotoDirection = Up;
				break;
			}
			if (stage->IsPassable(ex, ey + 1))
			{
				GotoDirection = Down;
				break;
			}


		case Up:
			if (stage->IsPassable(ex + 1, ey))
			{
				GotoDirection = Right;
				break;
			}
			if (stage->IsPassable(ex, ey - 1))
			{
				GotoDirection = Up;
				break;
			}
			if (stage->IsPassable(ex - 1, ey))
			{
				GotoDirection = Left;
				break;
			}


		case Down:
			if (stage->IsPassable(ex - 1, ey))
			{
				GotoDirection = Left;
				break;
			}
			if (stage->IsPassable(ex, ey + 1))
			{
				GotoDirection = Down;
				break;
			}
			if (stage->IsPassable(ex + 1, ey))
			{
				GotoDirection = Right;
				break;
			}
		}
		return GotoDirection;
	}
}
