//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include "Frameworks/GameContext.h"

#include "Frameworks/ResourceManager.h"
#include "Frameworks/GameFont.h"

#include "Game/SceneManager.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify, public GameContext
{
public:

    Game() noexcept(false);
	virtual ~Game() = default;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

	// <すべてのデータのために必要なものを最初に初期化>
	void FirstInit(HWND window, int width, int height);
	// <ゲームに必要なものを読み込む>
	void InitDatas(HWND window, int width, int height);
	// <ロード画面描画>
	void RenderInit(GameFont* font, int width, int height);

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear(const FLOAT* backColor = DirectX::Colors::CornflowerBlue);

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

	// <ロードの進捗>
	void Progress();
	// <遅延>
	void Delay();

	static const int PROGRESS_END;

    // Device resources.
    DX::DeviceResources*				    m_deviceResources;

	DirectX::SimpleMath::Vector2			m_windowSize;
	DirectX::SimpleMath::Vector2			m_windowCenter;

	int										m_initProgress = 0;
};