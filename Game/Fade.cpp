#include "Fade.h"

#include "../Frameworks/GameContext.h"

#include "../GameSystem.h"

void Fade::Initialize(GameContext & context)
{
	m_effectSprite = std::make_unique<GameSpriteEffect>();
	m_effectSprite->Load(context, L"Resources/Sprite/canvas_black.png");
	m_effectSprite->LoadShader(context, L"Resources/Shader/FadePS.cso");
}

void Fade::Update(GameContext & context)
{
	if (isStart)
	{
		if (m_count == 360)
		{
			m_count = 0;
			m_alpha = 0.f;
			isStart = false;
		}
		if (m_count < 180)
			m_alpha += 1.f / 180.f;
		if (m_count >= 180)
			m_alpha -= 1.f / 180.f;

		m_count++;
	}
	else
	{
		m_count = 0;
	}

	m_effectSprite->SetConstBuffer(m_alpha);
}

void Fade::Render(GameContext & context)
{
	DirectX::SimpleMath::Vector2 pos(GameSystem::GetWindowSize().x / 2, GameSystem::GetWindowSize().y / 2);
	m_effectSprite->Render2D(context, pos);
}
