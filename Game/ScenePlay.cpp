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

	auto& player = this->AddGameObject(L"Player");
	player->AddComponent<PlayerComponent>();

	auto& director = this->AddGameObject(L"Director");
	director->AddComponent<GameDirector>();

	this->InitializeGameObject(context);
}

void ScenePlay::Update(GameContext & context)
{
	auto key = context.Get<DirectX::Keyboard>().GetState();
	bool f3 = key.F3;
	bool f5 = key.F5;

	if (f3 && !f3_old)
	{
		if (DebugMode)
			DebugMode = false;
		else
			DebugMode = true;
	}
	if (f5 && !f5_old)
	{
		switch (CameraMode)
		{
		case CAMERA_MODE::Free:
		{
			CameraMode = CAMERA_MODE::FollowPlayer;
			auto camera = this->Find(L"Camera")->GetComponent<FixedCamera>();
			camera->SetTarget(this->Find(L"Player").GetWeakPtr().lock().get());
		}
			break;

		case CAMERA_MODE::FollowPlayer:
		{
			CameraMode = CAMERA_MODE::Free;
			auto camera = this->Find(L"Camera")->GetComponent<FixedCamera>();
			camera->SetTarget(nullptr);
		}
			break;
		}
	}

	this->UpdateGameObject(context);

	f3_old = f3;
	f5_old = f5;
}

void ScenePlay::Render(GameContext & context)
{
	this->RenderGameObject(context);

	if (DebugMode)
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
		}

		auto& player = this->Find(L"Player")->GetComponent<PlayerComponent>();
		font.Draw({ 0,32 }, DirectX::Colors::White, "Player : ( %3d,%3d )",
			(int)(player->GetGridPosition().x),
			(int)(player->GetGridPosition().y));
	}
}

void ScenePlay::Finalize(GameContext & context)
{
	this->FinalizeGameObject(context);
}


