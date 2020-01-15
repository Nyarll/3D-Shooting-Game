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
#include "PlayerComponent.h"
#include "EnemyComponent.h"

#include "StatusComponent.h"

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
	camera->GetComponent<FixedCamera>()->ModeNormal();

	auto& grid = this->AddGameObject(L"Grid");
	grid->AddComponent<GridComponent>();

	auto& sky = this->AddGameObject(L"Sky");
	sky->AddComponent<SkyComponent>();

	auto& stage = this->AddGameObject(L"Stage");
	stage->AddComponent<Stage>();

	auto& player = this->AddGameObject(L"Player");
	player->AddComponent<PlayerComponent>();
	player->AddComponent<StatusComponent>("Player", 100, 25, 30);
	player->GetComponent<StatusComponent>()->SetLevel(3);

	auto& enemy1 = this->AddGameObject(L"Enemy");
	enemy1->AddComponent<EnemyComponent>();

	auto& enemy2 = this->AddGameObject(L"Enemy");
	enemy2->AddComponent<EnemyComponent>();

	auto& director = this->AddGameObject(L"Director");
	director->AddComponent<GameDirector>();

	this->InitializeGameObject(context);
}

void ScenePlay::Update(GameContext & context)
{
	auto key = context.Get<DirectX::Keyboard>().GetState();
	bool f1 = key.F1;
	bool f3 = key.F3;
	bool f5 = key.F5;

	debugKey[0] = false;	// <ƒ_ƒ~[>

	if (f1 && !debugKey[1])
	{
		auto& stage = this->Find(L"Stage")->GetComponent<Stage>();
		stage->ChangeStageData(context);
	}

	if (f3 && !debugKey[3])
	{
		switch (DebugMode)
		{
		case DEBUG_MODE::ObjectDataMode:
			DebugMode = DEBUG_MODE::PlayerDataMode;
			break;
		case DEBUG_MODE::PlayerDataMode:
			DebugMode = DEBUG_MODE::NotDebugMode;
			break;
		case DEBUG_MODE::NotDebugMode:
			DebugMode = DEBUG_MODE::ObjectDataMode;
			break;
		}
	}
	if (f5 && !debugKey[5])
	{
		switch (CameraMode)
		{
		case CAMERA_MODE::Free:
		{
			CameraMode = CAMERA_MODE::FollowPlayer;
			auto camera = this->Find(L"Camera")->GetComponent<FixedCamera>();
			camera->SetTarget(this->Find(L"Player").GetWeakPtr().lock().get());
			camera->ModeNormal();
		}
		break;

		case CAMERA_MODE::FollowPlayer:
		{
			CameraMode = CAMERA_MODE::FPS;
			auto camera = this->Find(L"Camera")->GetComponent<FixedCamera>();
			camera->ModeFPS();
		}
		break;

		case CAMERA_MODE::FPS:
		{
			CameraMode = Free;
			auto camera = this->Find(L"Camera")->GetComponent<FixedCamera>();
			camera->ModeNormal();
			camera->SetTarget(nullptr);
		}
		break;
		}
	}

	this->UpdateGameObject(context);

	debugKey[1] = f1;
	debugKey[3] = f3;
	debugKey[5] = f5;
}

void ScenePlay::Render(GameContext & context)
{
	this->RenderGameObject(context);

	switch (DebugMode)
	{
	case ObjectDataMode:
		{
			auto& font = context.Get<GameFont>();
	
			font.Draw({ 0,0 }, DirectX::Colors::White, "Play Scene");
			auto camera = this->Find(L"Camera")->GetComponent<FixedCamera>();
			font.Draw({ 0,16 }, DirectX::Colors::White, "Camera : ( %.2f, %.2f )",
				(camera->GetEyePosition().x),
				(camera->GetEyePosition().z));
			switch (CameraMode)
			{
			case CAMERA_MODE::Free:
				font.Draw({ sizeof("Camera : ( 000.00, 000.00 )") * 9, 16 }, DirectX::Colors::White,
					"CameraMode : Free");
				break;
	
			case CAMERA_MODE::FollowPlayer:
				font.Draw({ sizeof("Camera : ( 000.00, 000.00 )") * 9, 16 }, DirectX::Colors::White,
					"CameraMode : Follow the player");
				break;
	
			case CAMERA_MODE::FPS:
				font.Draw({ sizeof("Camera : ( 000.00, 000.00 )") * 9, 16 }, DirectX::Colors::White,
					"CameraMode : FPS");
				break;
			}
	
			auto& player = this->Find(L"Player")->GetComponent<PlayerComponent>();
			font.Draw({ 0,32 }, DirectX::Colors::White, "Player : ( %3d,%3d )",
				(int)(player->GetGridPosition().x),
				(int)(player->GetGridPosition().y));
		}
		break;

	case PlayerDataMode:
		{
		auto& font = context.Get<GameFont>();
		auto& data = this->Find(L"Player")->GetComponent<StatusComponent>();
		font.Draw({ 0,0 }, DirectX::Colors::White, "Name : " + data->GetEntityName());
		font.Draw({ 0,20 }, DirectX::Colors::White, "Level : %d", data->GetLevel());
		font.Draw({ 0,40 }, DirectX::Colors::GreenYellow, "HP : ( %3d / %3d )", data->GetNowHP(), data->GetMaxHP());
		font.Draw({ 0,60 }, DirectX::Colors::Red, "ATK : %3d", data->GetATK());
		font.Draw({ 0,80 }, DirectX::Colors::Blue, "DEF : %3d", data->GetDEF());

		auto& player = this->Find(L"Player")->GetComponent<PlayerComponent>();
		font.Draw({ 0,120 }, DirectX::Colors::White, "Player : ( %3d,%3d )",
			(int)(player->GetGridPosition().x),
			(int)(player->GetGridPosition().y));
		}
		break;
	}
}

void ScenePlay::Finalize(GameContext & context)
{
	this->FinalizeGameObject(context);
}


