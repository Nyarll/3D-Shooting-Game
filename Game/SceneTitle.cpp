#include "SceneTitle.h"

#include "SceneManager.h"

#include "../Frameworks/GameFont.h"
#include "../Frameworks/GameSprite.h"

#include "Fade.h"

SceneTitle::SceneTitle()
{
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Initialize(GameContext & context)
{
	m_titleLogo = std::make_unique<GameSpriteEffect>();
	m_titleLogo->Load(context, L"Resources/Sprite/titleLogo.png");
	m_titleLogo->LoadShader(context, L"Resources/Shader/TitleSpritePS.cso");

	m_fade = std::make_unique<Fade>();
	m_fade->Initialize(context);
	m_fade->SetCount(180);
	m_fade->SetAlpha(1.f);

	m_fade->Start();
}

void SceneTitle::Update(GameContext & context)
{
	auto& manager = context.Get<SceneManager>();
	auto& key = context.Get<DirectX::Keyboard>().GetState();

	if (key.Space)
	{
		changePlayScene = true;
		m_fade->Start();
	}

	m_fade->Update(context);

	if (changePlayScene)
	{
		if (m_fade->GetCount() > m_fade->GetBlackoutTime())
		{
			manager.RequestScene(SceneID::SCENE_PLAY);
		}
	}
}

void SceneTitle::Render(GameContext & context)
{
	auto& font = context.Get<GameFont>();

	DirectX::SimpleMath::Vector2 logo_pos = DirectX::SimpleMath::Vector2(1280.f / 2.f, 720.f / 2.f);
	m_titleLogo->Render2D(context, logo_pos);

	DirectX::SimpleMath::Vector2 font_pos = { 1280.f / 2.f - static_cast<float>(sizeof("Push to [Space] Key.")) * 6.f, (720.f / 4.f) * 3.f };
	font.Draw(font_pos, DirectX::Colors::White, "Push to [Space] Key.");

	m_fade->Render(context);
}

void SceneTitle::Finalize(GameContext & context)
{
}
