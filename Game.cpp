//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "Frameworks/Object.h"

#include <thread>

#include "Game/ScenePlay.h"
#include "Game/SceneTitle.h"

#include "Frameworks/GameSprite.h"

#include "GameSystem.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

const int Game::PROGRESS_END = 6;

Game::Game() noexcept(false)
{
	Register(std::make_unique<DX::StepTimer>());
	Object::ObjectTime = float(Get<DX::StepTimer>().GetTotalSeconds());

	Register(std::make_unique<DX::DeviceResources>());
	m_deviceResources = &Get<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_windowSize.x = static_cast<float>(width);
	m_windowSize.y = static_cast<float>(height);
	m_windowCenter = m_windowSize / 2;
	// <DeviceResourcesの初期化など>
	this->FirstInit(window, width, height);

	// <ロード画面>
	{
		std::thread th1(&Game::InitDatas, this, window, width, height);
		std::thread th2(&Game::RenderInit, this, GetPtr<GameFont>(), width, height);
		th1.join();
		th2.join();
	}
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	auto& timer = Get<DX::StepTimer>();
	timer.Tick([&]()
	{
		Get<Keyboard>().GetState();
		Get<Mouse::ButtonStateTracker>().Update(Get<Mouse>().GetState());
		Update(timer);
	});

	Render();
}

void Game::FirstInit(HWND window, int width, int height)
{
	Register(std::make_unique<Mouse>());
	Get<Mouse>().SetWindow(window);
	Register(std::make_unique<Mouse::ButtonStateTracker>());
	Register(std::make_unique<Keyboard>());
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();
	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	Register(std::make_unique<GameFont>());
	Get<GameFont>().Load(*this, L"Resources/Fonts/Arial.spritefont");
}

void Game::InitDatas(HWND window, int width, int height)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();
	{
		// <コモンステートの作成>
		Register(std::make_unique<CommonStates>(device));
		this->Progress();

		// <エフェクトファクトリ>
		Register(std::make_unique<EffectFactory>(device));
		Get<EffectFactory>().SetDirectory(L"Resources/Models");
		this->Progress();
	}
	Register(std::make_unique<ResourceManager>());
	Get<ResourceManager>().Initialize(*this, window);
	this->Progress();

	Register(std::make_unique<SceneManager>());
	{
		auto& scene_manager = Get<SceneManager>();
		//scene_manager.RegisterScene(new [Scene]);

		scene_manager.RegisterScene(SceneID::SCENE_TITLE, SceneTitle::Create);
		this->Progress();

		scene_manager.RegisterScene(SceneID::SCENE_PLAY, ScenePlay::Create);
		this->Progress();

		scene_manager.SetStartScene(*this, SceneID::SCENE_TITLE);
		this->Progress();
	}

	// Progress : 6
}

void Game::RenderInit(GameFont* font, int width, int height)
{
	DirectX::SimpleMath::Vector2 pos((width / 2) - (width / 8), (height / 2));
	DirectX::SimpleMath::Vector2 str_pos(static_cast<float>(width - sizeof("Data Loading...") * 10), static_cast<float>(height - 40 * 1.5f));

	std::unique_ptr<GameSprite2D>	loadSprites[4];
	for (int i = 0; i < 4; i++)
	{
		loadSprites[i] = std::make_unique<GameSprite2D>();
	}
	loadSprites[0]->Load(*this, L"Resources/Sprite/load0.png");
	loadSprites[1]->Load(*this, L"Resources/Sprite/load1.png");
	loadSprites[2]->Load(*this, L"Resources/Sprite/load2.png");
	loadSprites[3]->Load(*this, L"Resources/Sprite/load3.png");

	int cnt = 0;
	while (m_initProgress < PROGRESS_END)
	{
		Clear(DirectX::Colors::White);
		m_deviceResources->PIXBeginEvent(L"Render");

		int n = 0;

		// <描画>
		if (cnt < 10)
		{
			n = 0;
		}
		else if (cnt < 20)
		{
			n = 1;
		}
		else if (cnt < 30)
		{
			n = 2;
		}
		else if (cnt < 40)
		{
			n = 3;
			cnt = 0;
		}
		
		loadSprites[n]->Draw(*this, str_pos);
		font->SetScale(0.75f);
		font->Draw(pos, DirectX::Colors::Black, "Now Progress : %2d / %2d", m_initProgress, PROGRESS_END);

		m_deviceResources->PIXEndEvent();

		// Show the new frame.
		m_deviceResources->Present();
		cnt++;
	}
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.
	elapsedTime;

	if (Get<DirectX::Keyboard>().GetState().Escape)
	{
		ExitGame();
	}

	Get<SceneManager>().UpdateActiveScene(*this);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (Get<DX::StepTimer>().GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");

	Get<SceneManager>().RenderActiveScene(*this);

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear(const FLOAT* backColor)
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, backColor);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	Get<DX::StepTimer>().ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 800;
	height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();

	// TODO: Initialize device dependent objects here (independent of window size).
	device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	// TODO: Initialize windows-size dependent objects here.
}

void Game::Progress()
{
	Delay();
	m_initProgress++;
}

void Game::Delay()
{
	if (!GameSystem::GetDebugMode())
	{
		for (int i = 0; i < 10000; i++)
		{
			for (int j = 0; j < 10000; j++);
		}
	}
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
