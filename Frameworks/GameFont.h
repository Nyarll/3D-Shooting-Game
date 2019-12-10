#pragma once

#include "GameContext.h"
#include <string>

// <GameFont>
class GameFont
{
private:
	std::unique_ptr<DirectX::CommonStates> m_state;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	float m_scale;

public:
	GameFont() = default;
	~GameFont() = default;

	bool Load(GameContext& ctx, const wchar_t* file_name, float scale = 0.75f);

	void Draw(DirectX::SimpleMath::Vector2 pos,DirectX::FXMVECTOR color, std::string fmt_str, ...);

	void SetScale(float scale) { m_scale = scale; }
};