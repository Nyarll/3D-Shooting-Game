#include "Stage.h"

#include "../Frameworks/GameContext.h"
#include "SceneManager.h"

#include "ModelRenderer.h"

#include "MapChip.h"
#include "FixedCameraComponent.h"
#include "PlayerComponent.h"

#include "../Frameworks/Random.h"

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
}

void Stage::Update(GameContext & context)
{
}

void Stage::Render(GameContext & context)
{
	auto& dr = context.GetDR();

	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto camera = scene.Find(L"Camera")->GetComponent<FixedCamera>();

	float scale = 0.0285f;

	for (int y = 0; y < m_stage.size(); y++)
	{
		for (int x = 0; x < m_stage[y].size(); x++)
		{
			if (m_stage[y][x].IsPassable())
			{
				DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(scale);
				world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_stage[y][x].GetPosition());
				m_model->Draw(dr.GetD3DDeviceContext(), context.Get<DirectX::CommonStates>(),
					world, camera->GetViewMatrix(), camera->GetProjectionMatrix());
			}
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
	while (true)
	{
		int x = random.Range(0, m_x);
		int y = random.Range(0, m_y);

		if (this->IsPassable(x, y))
		{
			auto& scene = context.Get<SceneManager>().GetActiveScene();
			auto& player = scene.Find(L"Player")->GetComponent<PlayerComponent>();

			player->SetGridPosition(context, { (float)x, (float)y });
			break;
		}
	}
}

void Stage::SpawnEnemy(GameContext & context)
{
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
