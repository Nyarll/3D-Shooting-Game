#include "Stage.h"

#include "../Frameworks/GameContext.h"
#include "SceneManager.h"

#include "ModelRenderer.h"

#include "MapChip.h"
#include "FixedCameraComponent.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"

#include "../Frameworks/Random.h"
#include "../Frameworks/ResourceManager.h"

const int Stage::mapSizeX = 50;
const int Stage::mapSizeY = 50;

Stage::Stage()
	: m_x(1)
	, m_y(1)
{
}

void Stage::Initialize(GameContext & context)
{
	assert((m_x != 0 || m_y != 0) && "Invalid value.");

	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& dr = context.GetDR();

	this->ChangeStageData(context);

	DirectX::EffectFactory factory(dr.GetD3DDevice());
	factory.SetDirectory(L"Resources/Models/Panel");
	m_model = DirectX::Model::CreateFromCMO(dr.GetD3DDevice(), L"Resources/Models/Panel/Panel1.cmo", factory);

	m_geometric = DirectX::GeometricPrimitive::CreateBox(dr.GetD3DDeviceContext(), DirectX::SimpleMath::Vector3(1.f, 0.25f, 1.f));
}

void Stage::Update(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
}

void Stage::Render(GameContext & context)
{
	auto& dr = context.GetDR();

	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	auto player = scene.Find(L"Player")->GetComponent<PlayerComponent>();
	DirectX::SimpleMath::Vector3 playerPos = player->gameObject->transform->position;
	int pgridX = static_cast<int>(playerPos.x);

	std::vector<std::vector<MapChip*>> data;

	// <描画のためのセッティング>
	for (int y = 0; y < m_stage.size(); y++)
	{
		data.push_back(std::vector<MapChip*>());
		for (int x = 0; x < m_stage[y].size(); x++)
		{
			if (m_stage[y][x].IsPassable())
			{
				int gridX = static_cast<int>(m_stage[y][x].GetPosition().x);
				int diffX = pgridX - gridX;

				if (diffX < 0)
					diffX *= (-1);

				m_stage[y][x].SetDrawPrio(diffX);
				data[y].push_back(&m_stage[y][x]);
			}
		}
	}
	// <描画順を決定>
	for (auto& d : data)
	{
		std::sort(d.begin(), d.end(),
			[](const MapChip* a, const MapChip* b)
		{
			return a->GetDrawPrio() > b->GetDrawPrio();
		});
	}

	float scale = 0.0285f;
	scale = 1.f;

	// <描画>
	for (auto& object : data)
	{
		for (auto& obj : object)
		{
			DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(scale);
			world *= DirectX::SimpleMath::Matrix::CreateTranslation(obj->GetPosition());

			m_geometric->Draw(world, camera->GetViewMatrix(), camera->GetProjectionMatrix(),
				DirectX::Colors::White, context.Get<ResourceManager>().GetTexture(ResourceManager::TextureID::Floor));
		}
	}
}

void Stage::ChangeStageData(GameContext& context)
{
	this->ResetMapData(mapSizeX, mapSizeY);

	this->SpawnPlayer(context);
	this->SpawnEnemy(context);
}

void Stage::SpawnPlayer(GameContext & context)
{
	Random random;
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& player = scene.Find(L"Player")->GetComponent<PlayerComponent>();
	auto& enemys = scene.FindAll(L"Enemy");

	std::vector<DirectX::SimpleMath::Vector2> enemysPosition;

	for (auto& e : enemys)
	{
		auto& ec = e->GetComponent<EnemyComponent>();
		DirectX::SimpleMath::Vector3 e_position = ec->gameObject->transform->position;
		DirectX::SimpleMath::Vector2 ep(e_position.x, e_position.z);
		enemysPosition.push_back(ep);
	}

	bool isOver = false;

	while (true)
	{
		int x = random.Range(0, m_x);
		int y = random.Range(0, m_y);

		isOver = false;

		for (auto& p : enemysPosition)
		{
			if (static_cast<int>(p.x) == x && static_cast<int>(p.y) == y)
			{
				isOver = true;
				break;
			}
		}

		if (!isOver)
		{
			if (this->IsPassable(x, y))
			{
				DirectX::SimpleMath::Vector3 pos(static_cast<float>(x), 0, static_cast<float>(y));
				player->gameObject->transform->localPosition = pos;
				break;
			}
		}
	}
}

void Stage::SpawnEnemy(GameContext & context)
{
	Random random;
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& player = scene.Find(L"Player")->GetComponent<PlayerComponent>();
	auto& enemys = scene.FindAll(L"Enemy");

	DirectX::SimpleMath::Vector2 playerPosition(player->gameObject->transform->localPosition.x, player->gameObject->transform->localPosition.z);
	std::vector<DirectX::SimpleMath::Vector2> enemysPosition;

	for (auto& e : enemys)
	{
		auto& ec = e->GetComponent<EnemyComponent>();
		DirectX::SimpleMath::Vector2 ep(ec->gameObject->transform->localPosition.x, ec->gameObject->transform->localPosition.z);
		enemysPosition.push_back(ep);
	}

	bool isOver = false;

	for (int i = 0; i < enemys.size(); i++)
	{
		while (true)
		{
			int x = random.Range(0, m_x);
			int y = random.Range(0, m_y);

			isOver = false;

			if (static_cast<int>(playerPosition.x) == x && static_cast<int>(playerPosition.y) == y)
			{
				isOver = true;
			}

			if (!isOver)
			{
				for (int j = 0; j < enemysPosition.size(); j++)
				{
					if (i != j)
					{
						if (static_cast<int>(enemysPosition[j].x) == x && static_cast<int>(enemysPosition[j].y) == y)
						{
							isOver = true;
							break;
						}
					}
				}
			}

			if (!isOver)
			{
				if (this->IsPassable(x, y))
				{
					enemys[i]->transform->localPosition = DirectX::SimpleMath::Vector3(static_cast<float>(x), 0, static_cast<float>(y));
					break;
				}
			}
		}
	}
}

void Stage::ResetMapData(int width, int height)
{
	// <配列内をすべて消去>
	if (m_stage.size() > 0)
	{
		for (int i = 0; i < m_stage.size(); i++)
		{
			m_stage[i].clear();
		}
	}
	m_stage.clear();

	m_x = width;
	m_y = height;

	// <ステージデータをとりあえず壁で埋める>
	for (int y = 0; y < m_y; y++)
	{
		m_stage.push_back(std::vector<MapChip>());
		for (int x = 0; x < m_x; x++)
		{
			m_stage[y].push_back(MapChip(x, y, false));
		}
	}

	// <マップを作成>
	this->CreateSpaceData();
}

void Stage::CreateSpaceData()
{
	Random random;

	int roomCount = random.Range(roomCountMin, roomCountMax);

	std::vector<int> meetPointsX;
	std::vector<int> meetPointsY;
	for (int i = 0; i < meetPointCount; i++)
	{
		meetPointsX.push_back(random.Range(m_x / 4, m_x * 3 / 4));
		meetPointsY.push_back(random.Range(m_y / 4, m_y * 3 / 4));
		m_stage[meetPointsY[i]][meetPointsX[i]].SetPassable(true);
	}

	for (int i = 0; i < roomCount; i++)
	{
		int roomWidth = random.Range(roomMinWidth, roomMaxWidth);
		int roomHeight = random.Range(roomMinHeight, roomMaxHeight);
		int roomPointX = random.Range(2, m_x - roomMaxWidth - 2);
		int roomPointY = random.Range(2, m_x - roomMaxWidth - 2);

		int roadStartPointX = random.Range(roomPointX, roomPointX + roomWidth);
		int roadStartPointY = random.Range(roomPointY, roomPointY + roomHeight);

		bool isRoad = this->CreateRoomData(roomWidth, roomHeight, roomPointX, roomPointY);
		if (!isRoad)
		{
			this->CreateRoadData(roadStartPointX, roadStartPointY,
				meetPointsX[random.Range(0, meetPointsX.size() - 1)],
				meetPointsY[random.Range(0, meetPointsY.size() - 1)]
			);
		}
	}
}

bool Stage::CreateRoomData(int roomWidth, int roomHeight, int roomPointX, int roomPointY)
{
	bool beCreateRoad = false;
	for (int y = 0; y < roomHeight; y++)
	{
		for (int x = 0; x < roomWidth; x++)
		{
			if (!m_stage[roomPointY + y][roomPointX + x].IsPassable())
			{
				m_stage[roomPointY + y][roomPointX + x].SetPassable(true);
			}
			else
			{
				beCreateRoad = true;
			}
		}
	}
	return beCreateRoad;
}

void Stage::CreateRoadData(int roadStartPointX, int roadStartPointY, int meetPointX, int meetPointY)
{
	bool isRight = false;
	if (roadStartPointX > meetPointX)
	{
		isRight = true;
	}

	bool isUnder = true;
	if (roadStartPointY > meetPointY)
	{
		isUnder = false;
	}
	Random random;
	if (random.Range(0, 2) == 0)
	{
		while (roadStartPointX != meetPointX)
		{
			m_stage[roadStartPointY][roadStartPointX].SetPassable(true);
			if (isRight)
			{
				roadStartPointX--;
			}
			else
			{
				roadStartPointX++;
			}
		}
		while (roadStartPointY != meetPointY)
		{
			m_stage[roadStartPointY][roadStartPointX].SetPassable(true);
			if (isUnder)
			{
				roadStartPointY++;
			}
			else
			{
				roadStartPointY--;
			}
		}
	}
	else
	{
		while (roadStartPointX != meetPointX)
		{
			m_stage[roadStartPointY][roadStartPointX].SetPassable(true);
			if (isRight)
			{
				roadStartPointX--;
			}
			else
			{
				roadStartPointX++;
			}
		}
		while (roadStartPointY != meetPointY)
		{
			m_stage[roadStartPointY][roadStartPointX].SetPassable(true);
			if (isUnder)
			{
				roadStartPointY++;
			}
			else
			{
				roadStartPointY--;
			}
		}
	}
}
