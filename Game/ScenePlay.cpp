#pragma warning(disable : 4239)

#include "ScenePlay.h"

#include "../DeviceResources.h"
#include "../Frameworks/ResourceManager.h"
#include "SceneManager.h"

#include "GridComponent.h"
#include "ModelRenderer.h"
#include "Sky.h"
#include "FixedCameraComponent.h"
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
	camera->AddComponent<FixedCamera>();

	auto& grid = this->AddGameObject(L"Grid");
	grid->AddComponent<GridComponent>();

	auto& sky = this->AddGameObject(L"Sky");
	sky->AddComponent<SkyComponent>();

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


