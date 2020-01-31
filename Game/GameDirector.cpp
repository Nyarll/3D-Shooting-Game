
#include "GameDirector.h"
#include "SceneManager.h"

#include "PlayerComponent.h"
#include "EnemyComponent.h"

#include "StatusComponent.h"

#include "../GameSystem.h"

#include "../Frameworks/GameFont.h"

void GameDirector::Initialize(GameContext & context)
{
	m_nowAct = ActionState::Idle;

	m_hpBar = std::make_unique<GameSpriteEffect>();
	m_hpBar->Load(context, L"Resources/Sprite/bar.png", 0.5f);
	m_hpBar->LoadShader(context, L"Resources/Shader/hpBarPS.cso", L"Resources/Shader/ParticleVS.cso", L"Resources/Shader/ProgressBarGS.cso");
}

void GameDirector::Update(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();

	switch (m_nowAct)
	{
	case ActionState::Idle:
		m_nowAct = ActionState::PlayerTurn;
		break;

	case ActionState::PlayerTurn:
	{
		auto& player = scene.Find(L"Player");
		if (player->GetComponent<PlayerComponent>()->Move(context))
		{
			m_nowAct = ActionState::EnemyTurn;
		}
	}
		break;

	case ActionState::EnemyTurn:
	{
		auto& enemys = scene.FindAll(L"Enemy");

		for (auto& e : enemys)
		{
			e->GetComponent<EnemyComponent>()->Move(context);
		}

		m_nowAct = ActionState::TurnEnd;
	}
		break;

	case ActionState::TurnEnd:
		m_nowAct = ActionState::Idle;
		break;
	}
	auto& data = scene.Find(L"Player")->GetComponent<StatusComponent>();
	m_hpBar->SetConstBuffer(data->GetNowHP(), data->GetMaxHP());
}

void GameDirector::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
	auto& font = context.Get<GameFont>();

	auto hpBarPos = DirectX::SimpleMath::Vector2((GameSystem::GetWindowSize().x / 2), GameSystem::GetWindowSize().y * 2 + GameSystem::GetWindowSize().y / 8);
	m_hpBar->Render2D(context, hpBarPos);
	auto& data = scene.Find(L"Player")->GetComponent<StatusComponent>();
	font.Draw(DirectX::SimpleMath::Vector2(GameSystem::GetWindowSize().x / 2 - sizeof("HP : 000 / 000") * 6, 60.f),
		DirectX::Colors::Black, "HP : %3d / %3d", data->GetNowHP(), data->GetMaxHP());

}
