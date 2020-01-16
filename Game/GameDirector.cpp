
#include "GameDirector.h"
#include "SceneManager.h"

#include "PlayerComponent.h"
#include "EnemyComponent.h"

void GameDirector::Initialize(GameContext & context)
{
	m_nowAct = ActionState::Idle;
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
}

void GameDirector::Render(GameContext & context)
{
	auto& scene = context.Get<SceneManager>().GetActiveScene();
}
