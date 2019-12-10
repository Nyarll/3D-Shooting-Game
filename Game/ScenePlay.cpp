#pragma warning(disable : 4239)

#include "ScenePlay.h"

#include "../Frameworks/DebugCamera.h"
#include "../Frameworks/InfinityGridFloor.h"

#include "../DeviceResources.h"
#include "../Frameworks/ResourceManager.h"
#include "SceneManager.h"

#include "GridComponent.h"
#include "GameCameraComponent.h"
#include "GameDirector.h"
#include "Stage.h"

#include "../Frameworks/GameFont.h"

ScenePlay::ScenePlay()
{
}

ScenePlay::~ScenePlay()
{
}

void ScenePlay::Initialize(GameContext & context)
{
	auto& camera = this->AddGameObject(L"Camera");
	camera->AddComponent<GameCameraComponent>();

	auto& grid = this->AddGameObject(L"Grid");
	grid->AddComponent<GridComponent>();

	auto& stage = this->AddGameObject(L"Stage");
	stage->AddComponent<Stage>();

	auto& director = this->AddGameObject(L"Director");
	director->AddComponent<GameDirector>();

	this->InitializeGameObject(context);
}

void ScenePlay::Update(GameContext & context)
{
	this->UpdateGameObject(context);
}

void ScenePlay::Render(GameContext & context)
{
	SphereCollider::IsDraw(true);
	this->RenderGameObject(context);
}

void ScenePlay::Finalize(GameContext & context)
{
	this->FinalizeGameObject(context);
}


