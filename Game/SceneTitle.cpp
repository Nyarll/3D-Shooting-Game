#include "SceneTitle.h"

#include "SceneManager.h"

#include "../Frameworks/GameFont.h"
#include "../Frameworks/GameSprite.h"

SceneTitle::SceneTitle()
{
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Initialize(GameContext & context)
{
	m_titleLogo = std::make_unique<GameSpriteEffect2D>();
	m_titleLogo->Load(context, L"Resources/Sprite/titleLogo.png");
}

void SceneTitle::Update(GameContext & context)
{
	auto& manager = context.Get<SceneManager>();
	auto& key = context.Get<DirectX::Keyboard>().GetState();

	if (key.Space)
	{
		manager.RequestScene(SceneID::SCENE_PLAY);
	}
}

void SceneTitle::Render(GameContext & context)
{
	auto& font = context.Get<GameFont>();

	DirectX::SimpleMath::Vector2 logo_pos = DirectX::SimpleMath::Vector2::Zero;
	//m_titleLogo->RenderFullWindow(context, DirectX::SimpleMath::Vector2(1280.f / 2.f, 720.f / 2.f));
	m_titleLogo->RenderFullWindow(context, logo_pos);

	DirectX::SimpleMath::Vector2 font_pos = { 1280.f / 2.f - static_cast<float>(sizeof("Push to [Space] Key.")) * 6.f, (720.f / 4.f) * 3.f };
	font.Draw(font_pos, DirectX::Colors::White, "Push to [Space] Key.");
}

void SceneTitle::Finalize(GameContext & context)
{
}
