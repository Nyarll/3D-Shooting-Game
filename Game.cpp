//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "Frameworks/Object.h"
#include "Frameworks/DebugCamera.h"
#include "Frameworks/GridFloorWrapper.h"

#include <thread>

#include "Game/ScenePlay.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

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
	Register(std::make_unique<Mouse>());
	Get<Mouse>().SetWindow(window);
	Register(std::make_unique<Mouse::ButtonStateTracker>());

	Register(std::make_unique<Keyboard>());

	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();
	{
		// <コモンステートの作成>
		Register(std::make_unique<CommonStates>(device));
		// <エフェクトファクトリ>
		Register(std::make_unique<EffectFactory>(device));
		Get<EffectFactory>().SetDirectory(L"Resources/Models");
		// <カメラ>
		// Register(std::make_unique<DebugCamera>());
		// Get<DebugCamera>().Initialize(*this);
		Register(std::make_unique<DebugFollowCamera>(
			DirectX::SimpleMath::Vector3(0, 2, -10),
			DirectX::SimpleMath::Vector3(0, 0, 0),
			window
			)
		);
		Get<DebugFollowCamera>().Initialize(*this);

		Register(std::make_unique<GridFloorWrapper>());
		Get<GridFloorWrapper>().Initialize(*this);
	}
	Register(std::make_unique<ResourceManager>());
	Get<ResourceManager>().Initialize(*this, window);

	Register(std::make_unique<SceneManager>());
	auto& scene_manager = Get<SceneManager>();
	//scene_manager.RegisterScene(new [Scene]);
	scene_manager.RegisterScene(SceneID::ScenePlay, ScenePlay::Create);

	scene_manager.SetStartScene(*this, SceneID::ScenePlay);

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
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
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
