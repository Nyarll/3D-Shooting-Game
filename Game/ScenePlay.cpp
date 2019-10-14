#include "ScenePlay.h"

#include "../Frameworks/DebugCamera.h"
#include "../Frameworks/InfinityGridFloor.h"

#include "../DeviceResources.h"
#include "../Frameworks/ResourceManager.h"
#include "SceneManager.h"

#include "GridComponent.h"
#include "SkyComponent.h"
#include "PlayerComponent.h"
#include "GameAreaComponent.h"

#include "../Frameworks/GameFont.h"

ScenePlay::ScenePlay()
{
}

ScenePlay::~ScenePlay()
{
}

void ScenePlay::Initialize(GameContext & context)
{
	auto& grid = this->AddGameObject(L"Grid");
	grid->AddComponent<GridComponent>();

	auto& area = this->AddGameObject(L"LimitArea");
	area->AddComponent<GameAreaComponent>();
	

	auto& sky = this->AddGameObject(L"Sky");
	sky->AddComponent<SkyComponent>();

	auto& player = this->AddGameObject(L"Player");
	player->AddComponent<PlayerComponent>();

	this->InitializeGameObject(context);
}

void ScenePlay::Update(GameContext & context)
{
	{
		auto player = Find(L"Player");
		context.Get<DebugFollowCamera>().SetTarget(player->transform->position);
		context.Get<DebugFollowCamera>().Update(context);

		auto& sky = Find(L"Sky")->GetComponent<SkyComponent>();
		sky->SetLocalPosition(player->GetComponent<PlayerComponent>()->GetLocalPosition());
	}

	this->UpdateGameObject(context);
}

void ScenePlay::Render(GameContext & context)
{
	this->RenderGameObject(context);

	auto player = Find(L"Player")->GetComponent<PlayerComponent>();
	auto& font = context.Get<GameFont>();
	font.Draw({ 0,0 }, DirectX::Colors::DarkBlue, "Speed : %.4f", player->GetSpeed());

}

void ScenePlay::Finalize(GameContext & context)
{
	this->FinalizeGameObject(context);
}

