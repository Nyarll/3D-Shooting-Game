#include "Stage.h"

#include "../Frameworks/GameContext.h"
#include "SceneManager.h"

#include "ModelRenderer.h"

#include "MapChip.h"
#include "FixedCameraComponent.h"
#include "PlayerComponent.h"

#include <thread>

Stage::Stage()
	: m_x(13)
	, m_y(13)
{
}

void Stage::Initialize(GameContext & context)
{
	assert((m_x != 0 || m_y != 0) && "Invalid value.");

	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& dr = context.GetDR();

	for (int y = 0; y < m_y; y++)
	{
		m_stage.push_back(std::vector<MapChip>());
		for (int x = 0; x < m_x; x++)
		{
			/*float _x = -(static_cast<float>(m_x) / 2.f) + static_cast<float>(x) + 0.5f;
			float _y = -(static_cast<float>(m_y) / 2.f) + static_cast<float>(y) + 0.5f;*/

			if (x % 2 || y % 2)
			{
				m_stage[y].push_back(MapChip(x, y, true));
			}
			else
			{
				m_stage[y].push_back(MapChip(x, y, false));
			}
		}
	}

	{
		this->SpawnPlayer(context);
	}
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
			DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(scale);
			world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_stage[y][x].GetPosition());

			if (m_stage[y][x].IsPassable())
			{
				m_model->Draw(dr.GetD3DDeviceContext(), context.Get<DirectX::CommonStates>(),
					world, camera->GetViewMatrix(), camera->GetProjectionMatrix());
			}
		}
	}
}

void Stage::SpawnPlayer(GameContext & context)
{
	while (true)
	{
		int x = rand() % m_x;
		int y = rand() % m_y;

		if (this->IsPassable(x, y))
		{
			auto& scene = context.Get<SceneManager>().GetActiveScene();
			auto& player = scene.Find(L"Player")->GetComponent<PlayerComponent>();

			player->SetGridPosition(context, { (float)x, (float)y });
			break;
		}
	}
}
